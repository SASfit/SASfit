import numpy as np
import cv2
from SASformats import SANSdata
import polarTransform as pT
from scipy.ndimage import geometric_transform
import matplotlib as mpl
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import scipy.integrate as integrate
class SASazimuthal:
    __doc__ = """
    SASazimuthal(method) needs as an argument a string to a valid method for calculating azimuthal data.
    
    The `SASazimuthal` class is used for calculating azimuthal averages of 2D data. It provides
    different methods for calculating the azimuthal average, such as using OpenCV (`cv2`), Scipy
    (`scipy`), meshgrid (`meshgrid`), or a custom implementation called `polarTransform`. The
    class takes a 2D array as input and calculates the azimuthal average using the specified
    method. It also provides methods for converting between Cartesian and polar coordinates, as
    well as calculating the intensity profile along the azimuthal direction.
    """
    __author__ = "Joachim Kohlbrecher"
    __copyright__ = "Copyright 2023, The SASfit Project"
    __license__ = "GPL"
    __version__ = "1.0"
    __maintainer__ = "Joachim Kohlbrecher"
    __email__ = "joachim.kohlbrecher@psi.ch"
    __status__ = "Production"
    
    algorithms = ["cv2", "scipy", "meshgrid", 'polarTransform']
    cartimg = []
    polarimg = []
    backcartimg = []
    psi = []
    Ipsi = []
    mode = "polarTransform"
    center = [1,1]
    Rrange = [0,128]
    order = 0
    polarres=360
    visibility = 0
    MSvisibility = 0
    offset = 0

    def __init__(self, array2D, mode=None, center=None,Rrange=None, polarres=None, order=None):
        try:
            self.cartimg = np.asarray(array2D)
            if self.cartimg.ndim != 2:
                raise RuntimeError(f'first argument is not a 2D array!')
        except:
            raise RuntimeError(f'first argument is not a 2D array!')
        if mode == None:
            self.mode = "polarTransform"
        elif mode not in self.algorithms:
            raise RuntimeError(f'unknown methods for calculating polar image')
        else:
            self.mode = mode
        if center == None:
            self.center = [self.cartimg.shape[0]/2,self.cartimg.shape[1]/2]
        else:
            self.center = center
        if Rrange == None:
            self.Rrange = [0,self.cartimg.shape[1]]
        else:
            self.Rrange = Rrange
        if order == None:
            self.order = 0
        else:
            self.order = order
        if polarres == None:
            self.polarres = 360
        else:
            self.polarres = polarres
                   
        if self.mode == "polarTransform":
            self.azimuthal_average_polarTransform()
        elif self.mode == "cv2":
            self.polarres = self.cartimg.shape[1]
            self.azimuthal_average_cv2()
        elif self.mode == "scipy":
            self.polarres = self.cartimg.shape[1]
            self.azimuthal_average_scipy()
        else:
            self.azimuthal_average_meshgrid()
    
    def showresults(self):
        ThisMin = np.min(self.cartimg)
        fig, ax = plt.subplots(1,3,figsize=(10, 3))
        fig.tight_layout()
        im = ax[0].imshow(np.log(1+self.cartimg-ThisMin), interpolation='none',cmap='turbo',origin='upper')
        plt.colorbar(im, ax=ax[0])
        im = ax[1].imshow(np.log(1+self.polarimg-ThisMin), interpolation='none',cmap='turbo',origin='upper',aspect=self.polarimg.shape[1]/self.polarimg.shape[0])
        plt.colorbar(im, ax=ax[1])
        ax[2].plot(self.psi,self.Ipsi)
        ax[2].set_ylim([0,1.2*np.max(self.Ipsi)])
        
        def MaierSaupe(x, A, Ibckg, kappa, Offset):
            kappa = np.abs(kappa)
            I0 = np.abs(Ibckg)
            MSnorm = integrate.quad(lambda x: np.exp(np.abs(kappa)*np.cos(x)**2), 0, np.pi/2.)
            return I0+np.abs(A)*np.pi/(2.0*MSnorm[0])* np.exp(np.abs(kappa)*np.cos((x-Offset)*np.pi/180)**2)
        
        parameters, covariance = curve_fit(MaierSaupe, self.psi, self.Ipsi, p0=[np.average(self.Ipsi),0.2*np.average(self.Ipsi),0.5, 0])
        while  parameters[3] > 180:
            parameters[3] = parameters[3]-180
        while  parameters[3] < 0:
            parameters[3] = parameters[3]+180
        parameters, covariance = curve_fit(MaierSaupe, self.psi, self.Ipsi, p0=parameters)
        #pp.pprint(parameters)
        ax[2].plot(self.psi, MaierSaupe(self.psi, *parameters))
        Dmin = np.min(self.Ipsi)
        Dmax = np.max(self.Ipsi)
        Davg = np.average(self.Ipsi)
        self.visibility = (Dmax-Dmin)/(Davg) 
        print(self.visibility, Dmax, Dmin)
        MSmin = np.min(MaierSaupe(self.psi, *parameters))
        MSmax = np.max(MaierSaupe(self.psi, *parameters))
        MSavg = np.average(MaierSaupe(self.psi, *parameters))
        self.MSvisibility = (MSmax-MSmin)/(MSavg)
        print(self.MSvisibility, MSmax, MSmin)
        self.offset = parameters[3]
        print(f"psi_shape{self.psi.shape}")
        ax[2].text(30.0, 0.05*np.max(self.Ipsi), f"mode={self.mode}\noffset={np.round(parameters[3],3)}\nMaier-Saupe aspect ratio={np.round(self.MSvisibility,3)}\ndata aspect ratio={np.round(self.visibility,3)}",backgroundcolor='orange',fontsize='x-small')
        print(f"mode = {self.mode}")
        print(f"shapeCart = {self.cartimg.shape}")
        print(f"shapePolar = {self.polarimg.shape}")
        print(f"center = {self.center}")
        print(f"Rrange = {self.Rrange}")
        print(f"order = {self.order}")
        print(f"polarres = {self.polarres}")
        print(f"visibility = {self.visibility}")
        print(f"MSvisibility = {self.MSvisibility}")
        print(f"offset = {self.offset}")
        print(f"min,max (psi) = {np.min(self.psi)},{np.max(self.psi)}")
        plt.show()

    def img2polar(self):
        def polar2cart(r, theta,center):
            x = r  * np.cos(theta) + center[0]
            y = r  * np.sin(theta) + center[1]
            return x, y
        
        max_radius = int(0.5*np.linalg.norm( self.cartimg.shape ))
        max_radius = self.cartimg.shape[1]
        imgMin = np.min(self.cartimg)
        theta , R = np.meshgrid(np.linspace(0, 2*np.pi, self.polarres), 
                                np.arange(0, max_radius))
        Xcart, Ycart = polar2cart(R, theta,[self.center[0],self.cartimg.shape[1]-1-self.center[1]])
        Xcart = Xcart.astype(int)
        Ycart = Ycart.astype(int)
        Yncart = np.where((Ycart < self.cartimg.shape[0]), Ycart, int(self.center[0]))
        Xncart = np.where((Ycart < self.cartimg.shape[0]), Xcart, int(self.center[1]))
        Ycart = np.where((Xncart < self.cartimg.shape[1]), Yncart, int(self.center[0]))
        Xcart = np.where((Xncart < self.cartimg.shape[1]), Xncart, int(self.center[1]))
        Yncart = np.where((Ycart >= 0), Ycart, int(self.center[0]))
        Xncart = np.where((Ycart >= 0), Xcart, int(self.center[1]))
        Ycart = np.where((Xncart >= 0), Yncart, int(self.center[0]))
        Xcart = np.where((Xncart >= 0), Xncart, int(self.center[1]))
        self.polarimg = self.cartimg[Ycart,Xcart]
        self.polarimg = np.reshape(self.polarimg,(max_radius,self.polarres))
        self.polarimg = (np.transpose(self.polarimg))

    def topolar(self):
        max_radius = 0.5*np.linalg.norm( self.cartimg.shape )
        def transform(coords):
            theta = 2.0*np.pi*coords[1] / (self.cartimg.shape[1] - 1.)
            radius = max_radius * coords[0] / self.cartimg.shape[0]
            radius = coords[0]
            i = self.cartimg.shape[1]-1-self.center[1] - radius*np.sin(theta)
            j = radius*np.cos(theta) + self.center[0]
            return i,j
        self.polarimg = np.flipud(np.transpose(geometric_transform(self.cartimg, transform, order=self.order,mode='nearest',prefilter=True)))

    """convert polar to cartesian"""
    def tocart(self):
        max_radius = 0.5*np.linalg.norm( self.polarimg.shape )
        def transform(coords):
            xindex,yindex = coords
            x0, y0 = (255,255)
            x = xindex - x0
            y = yindex - y0
            r = np.sqrt(x ** 2.0 + y ** 2.0)*( self.polarimg.shape[1]/max_radius)
            theta = np.arctan2(y, x,where=True)
            theta_index = (theta + np.pi) * self.polarimg.shape[1] / (2 * np.pi)
            return (r,theta_index)
        self.backcartimg = geometric_transform(self.polarimg, transform, order=self.order,mode='nearest',prefilter=True)

    def calc_Ipsi(self):
        self.psi = (self.polarres-np.arange(self.polarres)-1)*360/self.polarres
        self.Ipsi  = self.polarimg[:,self.Rrange[0]:self.Rrange[1]].sum(axis=1) 
        self.DIpsi = self.polarimg[:,self.Rrange[0]:self.Rrange[1]].std(axis=1)

    def azimuthal_average_cv2(self):
        """_summary_
        updates:
            self.polarimg, self.psi, self.Ipsi
        """
        self.polarimg = np.array(cv2.linearPolar(self.cartimg,
                                                 (self.center[0],self.cartimg.shape[1]-self.center[1]-1),
                                                 self.cartimg.shape[1],cv2.WARP_FILL_OUTLIERS))
        self.calc_Ipsi()

    def azimuthal_average_meshgrid(self):
        """_summary_
        updates:
            self.polarimg, self.psi, self.Ipsi
        """
        self.img2polar()
        self.calc_Ipsi()

    def azimuthal_average_polarTransform(self):
        """_summary_

        updates:
            self.polarimg, self.psi, self.Ipsi
        """
        self.polarimg, ptSettings = pT.convertToPolarImage(self.cartimg,
                                                           center=(self.center[0],self.cartimg.shape[1]-self.center[1]-1),
                                                           angleSize=self.polarres,
                                                           radiusSize=self.cartimg.shape[1],
                                                           initialRadius=0,
                                                           finalRadius=self.cartimg.shape[1],
                                                           order=self.order,
                                                           useMultiThreading=True)
        self.calc_Ipsi()

    def azimuthal_average_scipy(self):
        """_summary_

        updates:
            self.polarimg, self.psi, self.Ipsi
        """
        self.topolar()
        self.calc_Ipsi()

    def azimuthal_average(Data_array, Rmin=20,Rmax=50,zero=(-999,-999),slices=128):
        sorted_data=[]
        data = np.array(Data_array)
        if zero[0] == -999 or zero[1] == -999:
            zero[0] = data.shape[0]/2
            zero[1] = data.shape[1]/2
        for i in range (slices):
            sorted_data.append([])
        for i in range(data.shape[1]):
            for j in range(data.shape[0]):  
                d=np.sqrt((data.shape[0]-i+1-zero[1])**2+(j+1-zero[0])**2)
                if(d>R_min and d<R_max):
                    n=int((np.arctan2(j-zero[0],len(data[0])-i-zero[1])+np.pi)*(Angular_slices/(2*np.pi)))

                    try:
                        sorted_data[n].append(data[i][j])
                    except:
                        if (n==Angular_slices):
                            sorted_data[0].append(data[i][j])
                        else:
                            print("Error...")
        psi=np.linspace(-np.pi,np.pi,Angular_slices, endpoint=False)+np.pi/Angular_slices
        average=[]
        st_dev=[]
        for i in range(Angular_slices):
            average.append(np.mean(np.array(sorted_data[i])))
            st_dev.append(np.std(np.array(sorted_data[i])))
        return average, st_dev

ThisData = SANSdata('C:\\Users\\kohlbrecher\\switchdrive\\SANS\\user\\Saegesser\\20222544\\D0021227.018')
ThisMin = np.min(ThisData.BerSANS["%Counts,DetCounts"])
ThisMax = np.max(ThisData.BerSANS["%Counts,DetCounts"])
ThisMask = SANSdata('C:\\Users\\kohlbrecher\\switchdrive\\SANS\\user\\Saegesser\\20222544\\18m.sma')
MaskBerSANS = ThisMask.BerSANS['%Mask,DetMask']
ThisMin=np.min(np.where(1- MaskBerSANS,ThisData.BerSANS["%Counts,DetCounts"],ThisMax))
MaskedData=np.where(1- MaskBerSANS,ThisData.BerSANS["%Counts,DetCounts"],ThisMin)

AIntcv2 = SASazimuthal(MaskedData,mode='cv2',center=[61.3,62.42], Rrange=[30,50], order=0, polarres=128)
AIntcv2.showresults()
AIntscipy = SASazimuthal(MaskedData,mode='scipy',center=[61.3,62.42], Rrange=[30,50], order=0, polarres=360)
AIntscipy.showresults()
AIntpolarTransform = SASazimuthal(MaskedData,mode='polarTransform',center=[61.3,62.42], Rrange=[30,50], order=0, polarres=360)
AIntpolarTransform.showresults()
AIntmeshgrid = SASazimuthal(MaskedData,mode='meshgrid',center=[61.3,62.42], Rrange=[30,50],polarres=360)
AIntmeshgrid.showresults()


"""
import pprint
pp = pprint.PrettyPrinter(indent=4)
pp.pprint(ThisMask.fformat)
MaskBerSANS = ThisMask.BerSANS['%Mask,DetMask']
ThisMin=np.min(np.where(1- MaskBerSANS,ThisData.BerSANS["%Counts,DetCounts"],ThisMax))
MaskedData=np.where(1- MaskBerSANS,ThisData.BerSANS["%Counts,DetCounts"],ThisMin)
pimg, psi, Ipsi, DIpsi = azimuthal_average_cv2(MaskedData,Rmin=30, Rmax=50,zero=(61.3,62.42))
pimg[:,27:29] = ThisMin
pimg[:,51:53] = ThisMin
import matplotlib as mpl
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import scipy.integrate as integrate
fig, ax = plt.subplots(3,2,figsize=(10, 7))
fig.tight_layout()
im = ax[1,0].imshow(np.log(1+MaskedData-ThisMin), interpolation='none',cmap='Paired',origin='upper')
plt.colorbar(im, ax=ax[1, 0])
im = ax[0,1].imshow(np.log(1+pimg-ThisMin), interpolation='none',cmap='turbo',origin='upper')
plt.colorbar(im, ax=ax[0, 1])
#im = ax[1,1].imshow(np.log(1+pimg[:,30:60]-ThisMin), interpolation='none',cmap='turbo',origin='upper',aspect=1/4)
#plt.colorbar(im, ax=ax[1, 1])
ax[0,0].plot(psi,Ipsi)
ax[0,0].set_ylim([0,1.2*np.max(Ipsi)])
print(np.dot(Ipsi,np.log(Ipsi)))
def MaierSaupe(x, A, Ibckg, kappa, Offset):
    kappa = np.abs(kappa)
    I0 = np.abs(Ibckg)
    MSnorm = integrate.quad(lambda x: np.exp(np.abs(kappa)*np.cos(x)**2), 0, np.pi/2.)
    return I0+np.abs(A)*np.pi/(2.0*MSnorm[0])* np.exp(np.abs(kappa)*np.cos((x-Offset)*np.pi/180)**2)
parameters, covariance = curve_fit(MaierSaupe, psi, Ipsi, p0=[np.average(Ipsi),0.2*np.average(Ipsi),0.5, 0])
while  parameters[3] > 180:
    parameters[3] = parameters[3]-180
while  parameters[3] < 0:
    parameters[3] = parameters[3]+180
pp.pprint(parameters)
ax[0,0].plot(psi, MaierSaupe(psi, *parameters))
MSmin = np.min(MaierSaupe(psi, *parameters))
MSmax = np.max(MaierSaupe(psi, *parameters))
visibility = (MSmax-MSmin)/(MSmin)
ax[0,0].text(30.0, 1.5, f"offset={np.round(parameters[3],3)}\naspect ratio={np.round(visibility,3)}", bbox=dict(facecolor='orange', alpha=0.5))
MSmin = np.min(MaierSaupe(psi, *parameters))
MSmax = np.max(MaierSaupe(psi, *parameters))
visibility = (MSmax-MSmin)/(MSmin)
print(f"offset={np.round(parameters[3],5)}  aspectration={np.round(visibility,5)}")
#pimg, psi, Ipsi, DIpsi = azimuthal_average_polarTransform(MaskedData,Rmin=30, Rmax=50,zero=(61.3,62.42))
pimg, psi, Ipsi, DIpsi = azimuthal_average_scipy(MaskedData,Rmin=30, Rmax=50,zero=[61.3,62.42],order=0)
pimg[:,27:29] = ThisMin
pimg[:,51:53] = ThisMin
newPimg, ptSettings = pT.convertToCartesianImage(pimg,imageSize=[128,128],order=0,useMultiThreading=True)
ax[2,0].plot(psi,Ipsi)
ax[2,0].set_ylim([0,1.2*np.max(Ipsi)])
parameters, covariance = curve_fit(MaierSaupe, psi, Ipsi, p0=[np.average(Ipsi),0.2*np.average(Ipsi),0.5, 0])
while  parameters[3] > 180:
    parameters[3] = parameters[3]-180
while  parameters[3] < 0:
    parameters[3] = parameters[3]+180
ax[2,0].plot(psi, MaierSaupe(psi, *parameters))
MSmin = np.min(MaierSaupe(psi, *parameters))
MSmax = np.max(MaierSaupe(psi, *parameters))
visibility = (MSmax-MSmin)/(MSmin)
ax[2,0].text(30.0, 1.5, f"offset={np.round(parameters[3],3)}\naspect ratio={np.round(visibility,3)}", bbox=dict(facecolor='orange', alpha=1))
MSmin = np.min(MaierSaupe(psi, *parameters))
MSmax = np.max(MaierSaupe(psi, *parameters))
visibility = (MSmax-MSmin)/(MSmin)
print(f"offset={np.round(parameters[3],5)}  aspectration={np.round(visibility,5)}")
im = ax[2,1].imshow(np.log(1+pimg),cmap='turbo',interpolation='none',aspect=128/128)
plt.colorbar(im, ax=ax[2, 1])
im = ax[1,1].imshow(np.log(1+newPimg), interpolation='none',cmap='turbo',origin='upper')
plt.colorbar(im, ax=ax[1, 1])
plt.show()
pimg = img2polar(MaskedData, [61.3,62.42], int(64*np.sqrt(2)), initial_radius = None, phase_width = 360)
plt.imshow(np.log(1+np.flipud(np.transpose(pimg))), interpolation='none',cmap='turbo',origin='upper',aspect=int(64*np.sqrt(2))/360)
plt.show()
"""
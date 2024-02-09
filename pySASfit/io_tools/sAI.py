import numpy as np
import sys
import os
try:
    from SASformats import SANSdata
    import smoothing
except:
    sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(os.getcwd()), '..')))
    from pySASfit.io_tools.SASformats import SANSdata
    import pySASfit.tools.smoothing
import cv2
import fabio
import pyFAI, pyFAI.azimuthalIntegrator, pyFAI.detectors
from pyFAI.azimuthalIntegrator import AzimuthalIntegrator
from pyFAI.gui import jupyter
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
    (`scipy`), meshgrid (`meshgrid`), pyFAI ('pyFAI') or a custom implementation called `polarTransform`. The
    class takes a 2D array as input and calculates the azimuthal average using the specified
    method. It also provides methods for converting between Cartesian and polar coordinates, as
    well as calculating the intensity profile along the azimuthal direction.
    """
    __author__ = "Joachim Kohlbrecher"
    __copyright__ = "Copyright 2024 The SASfit Project"
    __license__ = "GPL"
    __version__ = "1.0"
    __maintainer__ = "Joachim Kohlbrecher"
    __email__ = "joachim.kohlbrecher@psi.ch"
    __status__ = "Production"
    
    algorithms = ["cv2", "scipy", "meshgrid", 'polarTransform', 'pyFAI']
    cartimg = []
    polarimg = []
    backcartimg = []
    psi = []
    Ipsi = []
    mode = "polarTransform"
    center = [1,1]
    Rrange = [0,128]
    distance = 1
    pixelsize = [7.5, 7.5]
    order = 0
    polarres=360
    visibility = 0
    MSvisibility = 0
    offset = 0
    MSparameters = []
    MScovariance = []
    MSIpsi = []
    SMIpsi = []
    name = 'unknown'
    output=None

    def __init__(self, array2D, mode=None, center=None,Rrange=None, polarres=None, order=None, distance=None, pixelsize=None):
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
            if mode == "pyFAI" and distance == None and pixelsize == None:
                raise RuntimeError(f'for the methods pyFAI both the sample detector distance as well as the pixel size needs to be supplied')
        if center == None:
            self.center = [self.cartimg.shape[0]/2.,self.cartimg.shape[1]/2.]
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
        elif self.mode == "pyFAI":
            self.azimuthal_average_pyFAI()
        else:
            self.azimuthal_average_meshgrid()
        self.calcAnsisotropy()
    
    def calcAnsisotropy(self):
        def MaierSaupe2(x, A1, A2, Ibckg, kappa1, kappa2, Offset):
            kappa1 = np.abs(kappa1)
            kappa1 = np.abs(kappa2)
            I0 = np.abs(Ibckg)
            MSnorm1 = integrate.quad(lambda x: np.exp(np.abs(kappa1)*np.cos(x)**2), 0, np.pi/2.)
            MSnorm2 = integrate.quad(lambda x: np.exp(np.abs(kappa2)*np.cos(x)**2), 0, np.pi/2.)
            return I0+np.abs(A1)*np.pi/(2.0*MSnorm1[0])* np.exp(np.abs(kappa1)*np.cos((x-Offset)*np.pi/180)**2) \
                     +np.abs(A2)*np.pi/(2.0*MSnorm2[0])* np.exp(np.abs(kappa2)*np.cos((x-Offset)*np.pi/180)**2)
        def MaierSaupe1(x, A1, Ibckg, kappa1, Offset):
            kappa1 = np.abs(kappa1)
            I0 = np.abs(Ibckg)
            MSnorm1 = integrate.quad(lambda x: np.exp(np.abs(kappa1)*np.cos(x)**2), 0, np.pi/2.)
            return I0+np.abs(A1)*np.pi/(2.0*MSnorm1[0])* np.exp(np.abs(kappa1)*np.cos((x-Offset)*np.pi/180)**2) 
        from pySASfit.tools.smoothing import smooth_data_gaussian_filter1d_wrap, smooth_data_fft
        #self.SMIpsi = smooth_data_savgol_n(self.Ipsi, 15, 2)
        self.SMIpsi = smooth_data_gaussian_filter1d_wrap(self.Ipsi, 3)
        #self.SMIpsi = smooth_data_fft(self.Ipsi, 0.1*np.average(self.Ipsi))
        Dmin = np.min(self.SMIpsi)
        Dmax = np.max(self.SMIpsi)
        Davg = np.average(self.SMIpsi)
        self.MSparameters, self.MScovariance = curve_fit(MaierSaupe1, self.psi, self.Ipsi, p0=[Dmax-Dmin,Dmin,0.3, self.psi[self.SMIpsi.argmax()]])
        while  self.MSparameters[3] > 180:
            self.MSparameters[3] = self.MSparameters[3]-180
        while  self.MSparameters[3] < 0:
            self.MSparameters[3] = self.MSparameters[3]+180
        self.MSparameters, self.MScovariance = curve_fit(MaierSaupe1, self.psi, self.Ipsi, p0=self.MSparameters)
        self.visibility = (Dmax-Dmin)/(Davg) 
        #print(self.visibility, Dmax, Dmin)
        self.MSIpsi = MaierSaupe1(self.psi, *self.MSparameters)
        MSmin = np.min(self.MSIpsi)
        MSmax = np.max(self.MSIpsi)
        MSavg = np.average(self.MSIpsi)
        self.MSvisibility = (MSmax-MSmin)/(MSavg)
        #print(self.MSvisibility, MSmax, MSmin)
        self.offset = self.MSparameters[3]
        
    def showresults(self):
        ThisMin = np.min(self.cartimg)
        fig, ax = plt.subplots(1,3,figsize=(10, 3))
        fig.tight_layout()
        im = ax[0].imshow(np.log1p(self.cartimg-ThisMin), interpolation='none',cmap='turbo',origin='upper')
        plt.colorbar(im, ax=ax[0])
        im = ax[1].imshow(np.log1p(self.polarimg-ThisMin), interpolation='none',cmap='turbo',origin='upper',aspect=self.polarimg.shape[1]/self.polarimg.shape[0])
        plt.colorbar(im, ax=ax[1])
        ax[2].plot(self.psi,self.Ipsi)
        ax[2].set_ylim([0,1.2*np.max(self.Ipsi)])
        ax[2].plot(self.psi, self.MSIpsi)
        ax[2].plot(self.psi, self.SMIpsi)
        
        #print(f"psi_shape{self.psi.shape}")
        ax[2].set(title=f"mode={self.mode}\noffset={np.round(self.MSparameters[3],3)}\nkappa={np.round(self.MSparameters[2],3)}\nMaier-Saupe aspect ratio={np.round(self.MSvisibility,3)}\ndata aspect ratio={np.round(self.visibility,3)}")
        #ax[2].text(30.0, 0.05*np.max(self.Ipsi), f"mode={self.mode}\noffset={np.round(self.MSparameters[3],3)}\nMaier-Saupe aspect ratio={np.round(self.MSvisibility,3)}\ndata aspect ratio={np.round(self.visibility,3)}",backgroundcolor='orange',fontsize='x-small')
        #print(f"mode = {self.mode}")
        #print(f"shapeCart = {self.cartimg.shape}")
        #print(f"shapePolar = {self.polarimg.shape}")
        #print(f"center = {self.center}")
        #print(f"Rrange = {self.Rrange}")
        #print(f"order = {self.order}")
        #print(f"polarres = {self.polarres}")
        #print(f"visibility = {self.visibility}")
        #print(f"MSvisibility = {self.MSvisibility}")
        #print(f"offset = {self.offset}")
        #print(f"min,max (psi) = {np.min(self.psi)},{np.max(self.psi)}")
        #plt.tight_layout()
        if self.output == "show":
            plt.show()
        elif self.output == "png":
            plt.savefig(f"{self.name}.png", bbox_inches="tight")
        elif self.output == "png":
            plt.savefig(f"{self.name}.pdf", bbox_inches="tight")
        else:
            pass
        
          
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

    def azimuthal_average_pyFAI(self):
        """_summary_

        updates:
            self.polarimg, self.psi, self.Ipsi
        """
        """
        self.polarimg, FAISettings = pyFAI (self.cartimg,
                                                           center=(self.center[0],self.cartimg.shape[1]-self.center[1]-1),
                                                           angleSize=self.polarres,
                                                           radiusSize=self.cartimg.shape[1],
                                                           initialRadius=0,
                                                           finalRadius=self.cartimg.shape[1],
                                                           order=self.order,
                                                           useMultiThreading=True)
        """
        detector = pyFAI.detectors.Detector(self.pixelsize[0]/1000,self.pixelsize[1]/1000)
        detector.max_shape=self.cartimg.shape
        ai = AzimuthalIntegrator(dist=self.distance, detector=detector)
        ai.poni1=ai.pixel1*(self.cartimg.shape[0]-self.center[0]-1)
        #ai.poni1=ai.pixel1*self.center[0]
        ai.poni2=ai.pixel2*(self.cartimg.shape[1]-self.center[1]-1)
        ai.poni2=ai.pixel2*(self.center[1])
        self.polarimg, tth, self.psi = ai.integrate2d(self.cartimg, self.cartimg.shape[1], self.polarres,unit="2th_deg")
        self.polarimg = np.flipud(self.polarimg)
        self.psi, self.Ipsi = ai.integrate_radial(self.cartimg, self.polarres, \
                                                  npt_rad=self.cartimg.shape[1], \
                                                  correctSolidAngle=True, azimuth_range=(-180,180),\
                                                  radial_range=(ai.pixel1*self.Rrange[0]*1e3,ai.pixel2*self.Rrange[1]*1e3),\
                                                  method='csr', radial_unit='r_mm')
        self.Ipsi = self.Ipsi*(self.Rrange[1]-self.Rrange[0])
        self.psi = ((-self.psi)+360) % 360
        ind = np.argsort(self.psi)
        self.psi = self.psi[ind]
        self.Ipsi = self.Ipsi[ind]
        
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


AIntcv2 = SASazimuthal(MaskedData,mode='cv2',center=[61.3,62.42], Rrange=[45,50], order=0, polarres=128)
AIntcv2.showresults()
AIntscipy = SASazimuthal(MaskedData,mode='scipy',center=[61.3,62.42], Rrange=[45,50], order=0, polarres=360)
AIntscipy.showresults()
AIntpolarTransform = SASazimuthal(MaskedData,mode='polarTransform',center=[61.3,62.42], Rrange=[45,50], order=0, polarres=360)
AIntpolarTransform.showresults()
AIntmeshgrid = SASazimuthal(MaskedData,mode='meshgrid',center=[61.3,62.42], Rrange=[45,50],polarres=360)
AIntmeshgrid.showresults()
AIntmeshgrid = SASazimuthal(MaskedData,mode='pyFAI',center=[61.3,62.42], Rrange=[45,50],polarres=180, pixelsize=[7.5,7.5], distance=ThisData.BerSANS["%Setup,SD"])
AIntmeshgrid.showresults()

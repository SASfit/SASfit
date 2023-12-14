def azimuthal_average(BerSANS, Rmin=12,Rmax=50,slices=100):
    sorted_data=[]
    for i in range (slices):
        sorted_data.append([])
    for i in range(len(data)):
        for j in range(len(data[0])):  
            d=np.sqrt((len(data[0])-i+1-zero[1])**2+(j+1-zero[0])**2)
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

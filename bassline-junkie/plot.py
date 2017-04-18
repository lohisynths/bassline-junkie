
import matplotlib
from numpy import maximum
matplotlib.use('Qt4Agg')

import matplotlib.pyplot as plt
import numpy as np


import os


for file in os.listdir("."):
    if file.endswith(".bin"):
    
        array = np.fromfile(open(file, "r"), dtype=np.float64)
        
        
        
        ################################## raw data
        
        max_index = array.argmax()
        max_value = array[max_index]
        name = "max %f" % (max_value)
        
        
        plt1 = plt.subplot(2, 1, 1)
        plt1.plot(array, alpha=0.75)
        plt1.set_title('raw samples')
        plt1.set_xlabel('samples', fontsize=9)
        plt1.set_ylabel('miliseconds', fontsize=9)
        plt1.set_ylim([0, 10])
        # Plot maximum value
        plt1.annotate(name,
                    xy=(max_index, max_value), xycoords='data',
                    xytext=(10, 10), textcoords='offset points',
                    arrowprops=dict(arrowstyle="->"))
        
        ################################## histogram
        
        
        plt2 = plt.subplot(2, 1, 2)
        plt2.set_title('histogram of data')
        plt2.set_xlabel('miliseconds', fontsize=9)
        plt2.set_ylabel('probability', fontsize=9)
        
        hist_y, hist_x, _ = plt2.hist(array, bins=200, alpha=0.75)
        
        hist_max_y = hist_y.max()
        hist_max_x = hist_x.max()
        
        mean = np.mean(array)
        name = 'mean %f' % mean
         
        plt2.axvline(x=mean, color='red', linestyle='--', alpha=0.75)
        plt2.text(mean + mean * 0.005, hist_max_y - hist_max_y * 0.1, name, rotation=0, color='red', alpha=0.75)
           
        plt2_maximum = max(array)
        name = 'max %f' % plt2_maximum
        
        plt2.axvline(x=plt2_maximum, color='red', linestyle='--', alpha=0.75)
        plt2.text(
            plt2_maximum - plt2_maximum * 0.005,
            hist_max_y - hist_max_y * 0.1,
            name,
            rotation=0,
            color='red',
            alpha=0.75,
            horizontalalignment='right')
           
        plt.tight_layout()
        
        filename = os.path.splitext(file)[0] + ".png"
        print( filename )
        
        plt.savefig(filename)
        plt.cla()
        plt.clf()
        #plt.show()

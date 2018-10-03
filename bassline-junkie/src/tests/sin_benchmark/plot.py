import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os

folder = "./"

for file in os.listdir(folder):
    if file.endswith(".bin"):
        raw_data_type = np.float64
        if 'float' in file:
            raw_data_type = np.float32
        array = np.fromfile(open(folder + file, "r"), dtype=raw_data_type)

        plt1 = plt.subplot()
        plt1.plot(array, alpha=0.75)
        plt1.set_title(file)
        plt1.set_xlabel('raw samples', fontsize=9)
        plt1.set_ylabel('value', fontsize=9)
        plt.tight_layout()

        filename = os.path.splitext(file)[0] + ".png"
        print( filename )
        
        plt.savefig(folder + filename)
        plt.show()
        plt.cla()
        plt.clf()

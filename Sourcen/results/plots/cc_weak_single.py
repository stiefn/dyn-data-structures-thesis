import numpy as np
import matplotlib.pyplot as plt

plt.plot([2, 4, 8, 16, 28], 
         [11.71, 14.16, 18.79, 20.83, 25.48], 'k-', label='')
plt.axis([0, 29, 0, 27])
plt.xticks( np.append(2**np.arange(5), 28) )
plt.grid(True)
plt.xlabel('number of processors')
plt.ylabel('time (s)')

plt.show()
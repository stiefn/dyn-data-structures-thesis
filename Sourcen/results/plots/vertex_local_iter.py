import numpy as np
import matplotlib.pyplot as plt

plt.plot([1, 2, 4, 8, 16], 
         [1.16, 1.19, 1.21, 1.17, 1], 'k-', label='1M Vertices')
		 
plt.plot([1, 2, 4, 8, 16], 
         [11.87, 11.86, 11.84, 11.81, 11.9], 'k--', label='10M Vertices')
		 
plt.legend(loc='upper right')
plt.axis([1, 16, 0, 15])
plt.xticks( 2**np.arange(4) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')

plt.show()
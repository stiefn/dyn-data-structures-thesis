import numpy as np
import matplotlib.pyplot as plt

plt.plot([1, 2, 4, 8, 16], 
         [2.67, 2.67, 2.67, 2.67, 2.67], 'k-', label='1M Vertices')
		 
plt.plot([1, 2, 4, 8, 16], 
         [26.68, 26.7, 26.67, 26.68, 26.69], 'k--', label='10M Vertices')
		 
plt.legend(loc='upper right')
plt.axis([1, 16, 0, 33])
plt.xticks( 2**np.arange(4) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')

plt.show()
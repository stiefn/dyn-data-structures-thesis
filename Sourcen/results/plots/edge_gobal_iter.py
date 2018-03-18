import numpy as np
import matplotlib.pyplot as plt

plt.plot([1, 2, 4, 8, 16], 
         [2.75, 2.74, 2.74, 2.76, 2.75], 'k-', label='1M Edges')
		 
plt.plot([1, 2, 4, 8, 16], 
         [27.51, 27.34, 27.54, 27.54, 27.5], 'k--', label='10M Edges')
		 
plt.legend(loc='upper right')
plt.axis([1, 16, 0, 34])
plt.xticks( 2**np.arange(4) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')

plt.show()
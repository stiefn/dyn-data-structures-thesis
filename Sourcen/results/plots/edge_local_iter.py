import numpy as np
import matplotlib.pyplot as plt

plt.plot([1, 2, 4, 8, 16], 
         [1.86, 1.88, 1.9, 1.88, 2.05], 'k-', label='1M Edges')
		 
plt.plot([1, 2, 4, 8, 16], 
         [18.64, 18.65, 18.58, 18.5, 18.58], 'k--', label='10M Edges')
		 
plt.legend(loc='upper right')
plt.axis([1, 16, 0, 23])
plt.xticks( 2**np.arange(4) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')

plt.show()
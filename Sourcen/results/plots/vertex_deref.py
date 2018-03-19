import numpy as np
import matplotlib.pyplot as plt

plt.plot([1, 2, 4, 8], 
         [0.01, 33.12, 63.93, 225.49], 'k-', label='448 Vertices')
		 
plt.plot([1, 2, 4, 8], 
         [0.02, 58.91, 82.38, 186.65], 'k--', label='896 Vertices')
		 
plt.legend(loc='upper left')
plt.axis([1, 8, 0, 240])
plt.xticks( 2**np.arange(4) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')

plt.show()
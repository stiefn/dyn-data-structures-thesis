import numpy as np
import matplotlib.pyplot as plt

plt.plot([4, 8, 16, 32], 
         [110.29, 139.24, 204.62, 348.35], 'k-', label='Equal Dist.')
		 
plt.plot([4, 8, 16, 32, 64], 
         [89.68, 105.11, 136.82, 218.37, 395.23], 'k--', label='Log. Dist.')
		 
plt.legend(loc='lower right')
plt.axis([3, 65, 0, 400])
plt.xticks( 2**np.arange(2, 7) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')

plt.show()
import numpy as np
import matplotlib.pyplot as plt

plt.figure(figsize=(5,4))
plt.plot([4, 8, 16, 32, 64], 
         [32.34, 34.55, 39.86, 42.59, 54.59], 'k-', label='Equal Dist.')
		 
plt.plot([4, 8, 16, 32, 64], 
         [27.09, 27.96, 30.5, 34.5, 49.45], 'k--', label='Log. Dist.')
		 
plt.legend(loc='lower right')
plt.axis([4, 64, 0, 57])
plt.xticks( 2**np.arange(2, 7) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')
plt.tight_layout()

plt.show()
import numpy as np
import matplotlib.pyplot as plt

plt.figure(figsize=(5,4))
plt.plot([8, 16, 32, 64], 
         [72.13, 38.28, 22.24, 16.54], 'k-', label='Equal Dist.')
		 
plt.plot([8, 16, 32, 64], 
         [58.44, 30.96, 16.59, 11.93], 'k--', label='Log. Dist.')
		 
upc_line = plt.plot([8, 64], [19.5, 19.5], 'b--', label='UPC CC 16N')
plt.setp(upc_line, 'linewidth', 2.0)

upc_line = plt.plot([8, 64], [43, 43], 'r--', label='SMP')
plt.setp(upc_line, 'linewidth', 2.0)
		 
plt.legend(loc='upper right')
plt.axis([8, 64, 0, 74])
plt.xticks( 2**np.arange(3, 7) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')
plt.tight_layout()

plt.show()
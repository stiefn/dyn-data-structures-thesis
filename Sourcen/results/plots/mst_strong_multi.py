import numpy as np
import matplotlib.pyplot as plt

plt.figure(figsize=(5,4))
plt.plot([8, 16, 32, 64], 
         [303.94, 203.59, 164.36, 171.08], 'k-', label='Equal Dist.')
		 
plt.plot([8, 16, 32, 64], 
         [224.06, 136.21, 103.13, 93.34], 'k--', label='Log. Dist.')
		 
upc_line = plt.plot([8, 64], [88, 88], 'b--', label='UPC MST 16N')
plt.setp(upc_line, 'linewidth', 2.0)

upc_line = plt.plot([8, 64], [900, 900], 'r--', label='SMP')
plt.setp(upc_line, 'linewidth', 2.0)
		 
plt.legend(loc='center right')
plt.axis([8, 64, 0, 950])
plt.xticks( 2**np.arange(3, 7) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')
plt.tight_layout()

plt.show()
import numpy as np
import matplotlib.pyplot as plt

plt.figure(figsize=(10,4))
plt.subplot(121)
plt.plot([1, 2, 4, 8, 16], 
         [2.67, 2.67, 2.67, 2.67, 2.67], 'k-', label='1M Vertices')
		 
plt.plot([1, 2, 4, 8, 16], 
         [26.68, 26.7, 26.67, 26.68, 26.69], 'k--', label='10M Vertices')
		 
plt.legend(loc='center right')
plt.axis([1, 16, 0, 30])
plt.xticks( 2**np.arange(4) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')

plt.subplot(122)
plt.plot([1, 2, 4, 8, 16], 
         [2.75, 2.74, 2.74, 2.76, 2.75], 'k-', label='1M Edges')
		 
plt.plot([1, 2, 4, 8, 16], 
         [27.51, 27.34, 27.54, 27.54, 27.5], 'k--', label='10M Edges')
		 
plt.legend(loc='center right')
plt.axis([1, 16, 0, 30])
plt.xticks( 2**np.arange(4) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.tight_layout()

plt.show()
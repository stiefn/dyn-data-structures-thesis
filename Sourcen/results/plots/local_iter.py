import numpy as np
import matplotlib.pyplot as plt

plt.figure(figsize=(10,4))
plt.subplot(121)
plt.plot([1, 2, 4, 8, 16], 
         [1.16, 1.19, 1.21, 1.17, 1], 'k-', label='1M Vertices')
		 
plt.plot([1, 2, 4, 8, 16], 
         [11.87, 11.86, 11.84, 11.81, 11.9], 'k--', label='10M Vertices')
		 
plt.legend(loc='center right')
plt.axis([1, 16, 0, 13])
plt.xticks( 2**np.arange(4) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')

plt.subplot(122)
plt.plot([1, 2, 4, 8, 16], 
         [1.86, 1.88, 1.9, 1.88, 2.05], 'k-', label='1M Edges')
		 
plt.plot([1, 2, 4, 8, 16], 
         [18.64, 18.65, 18.58, 18.5, 18.58], 'k--', label='10M Edges')
		 
plt.legend(loc='center right')
plt.axis([1, 16, 0, 20.5])
plt.xticks( 2**np.arange(4) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.tight_layout()

plt.show()
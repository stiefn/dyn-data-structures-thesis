import numpy as np
import matplotlib.pyplot as plt

plt.plot([1, 2, 4, 8], 
         [0.01, 28.78, 38.97, 81.26], 'k-', label='448 Edges')
		 
plt.plot([1, 2, 4, 8], 
         [0.03, 59.31, 82.85, 187.77], 'k--', label='896 Edges')
		 
plt.legend(loc='upper left')
plt.axis([1, 8, 0, 200])
plt.xticks( 2**np.arange(4) )
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')

plt.show()
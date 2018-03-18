import numpy as np
import matplotlib.pyplot as plt

plt.plot([100000, 1000000, 10000000], 
         [0.2, 1.8, 21.37], 'k-', label='Vertex insert.')
		 
plt.plot([100000, 1000000, 10000000], 
         [0.1, 1, 10.07], 'k--', label='Edge insert.')
		 
plt.xscale('log')
plt.legend(loc='upper left')
plt.axis([100000, 10000000, 0, 22])
plt.grid(True)
plt.xlabel('number of nodes')
plt.ylabel('time (s)')

plt.show()
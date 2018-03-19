import numpy as np
import matplotlib.pyplot as plt

plt.figure(figsize=(5,4))
plt.plot([2, 4, 8, 16, 28], 
         [37.13, 42.66, 48.54, 60.46, 71.04], 'k-', label='')
plt.axis([2, 28, 0, 72])
plt.xticks( np.append(2**np.arange(1,5), 28) )
plt.grid(True)
plt.xlabel('number of processors')
plt.ylabel('time (s)')
plt.tight_layout()

plt.show()
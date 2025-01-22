import matplotlib.pyplot as plt

val_min = [32, 31, 29, 29, 31, 30, 31, 5, 5, 0, 0]
val_mid = [24, 30, 30, 29, 31, 31, 29, 34, 34, 17, 0]
val_max = [15, 21, 30, 30, 32, 35, 30, 32, 32, 38, 17]

plt.plot(val_min, label="IrP1")
plt.plot(val_mid, label="IrP2")
plt.plot(val_max, label="IrP3")

plt.xlabel('Distance (cm)')
plt.ylabel('Nb messages received')
plt.title('Number of messages received by distance')
plt.legend()

plt.show()
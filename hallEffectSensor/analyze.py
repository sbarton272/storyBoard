
import matplotlib.pyplot as plt
import csv

FID = open('magnetData.txt', 'r')
reader = csv.reader(FID)

fig=plt.figure()

y=list()
for line in reader:
	y.append(int(line[0].strip()))
x=range(0,len(y))

# derivative
dy = list()
for i in xrange(0,len(y)-1):
	dy.append( y[i+1] - y[i] )

FID.close()

plt.ion()
plt.show()

fig1 = plt.figure()
ax1 = fig1.add_subplot(111)
ax1.scatter(x, y, marker='.')
ax1.set_title('raw data')

fig2 = plt.figure()
ax2 = fig2.add_subplot(111)
ax2.scatter(x[0:-1], dy, marker='.')
ax2.set_title('derivative')



plt.show()

wait = raw_input("PRESS ENTER TO CONTINUE.")
print("done")

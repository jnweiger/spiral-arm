#! /usr/bin/python3
# compute length of the spiral arm
#  -> 1.5m 

l1=110          # length of first segment mm
n=27            # number of segments
downscale=0.945 # scale factor from segment to segment

l = []
for i in range(n):
  l.append(l1)
  l1 = l1 * downscale

print(sum(l))

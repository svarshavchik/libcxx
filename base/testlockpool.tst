Stress test
Ok
alock1: 1, 1
alock2: 0
alock3: 0
block1: 1
alock2: 0
alock2: 1, 1
clock: 1, 1
multilock1: 1
multilock2: 0
multilock3: 0
multilock2: 1
multilock3: 0
multilock3: 1
---
multilock1: 1
multilock2: 0
multilock3: 1
multilock2: 0
multilock3: 1
multilock2: 1
---
readlock: readlock1: 1
readlock: writelock2: 0
readlock: readlock3: 0
readlock: writelock2: 1
readlock: readlock3: 0
readlock: readlock3: 1
---
readlock: readlock1: 1
readlock: writelock2: 0
readlock: readlock3: 1
readlock: writelock2: 0
readlock: readlock3: 1
readlock: writelock2: 1

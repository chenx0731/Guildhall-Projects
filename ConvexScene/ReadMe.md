- Convex Scene - Chen Xu

-----------------------------

1. 1024 objects vs 1024 raycasts with disc2 tree partitioning at 30 fps in release build, 2048 objects vs 1024 raycasts without partitioning at 30 fps in release build.

2. It gets worse when I enable partitioning scheme, because the nodes(disc2) of the tree don't divide the space nicely, and they overlap with each other too much and the size of them is big. So when doing raycast from the root of the tree, it can't cut much branches. Without partitioning, there are N objects, each raycast needs to test N times, with unoptimized partitioning, there are 2N - 1 nodes to check for each raycast. Therefore, it worsens the performance.

3.   

   |                                                   | Debug | Release |
   | ------------------------------------------------- | ----- | ------- |
   | Object: 32; Raycast: 1024; With Partitioning      | 30fps | 60fps   |
   | Object: 32; Raycast: 1024; Without Partitioning   | 60fps | 60fps   |
   | Object: 1024; Raycast: 1024; With Partitioning    | 10fps | 30fps   |
   | Object: 1024; Raycast: 1024; Without Partitioning | 10fps | 60fps   |
   | Object: 2048; Raycast: 1024; Without Partitioning | 10fps | 30fps   |

4. With partitioning, objects N, raycasts M, O(2N * M), without partitioning O(N * M)
5. If there are N objects in the scene, the depth of disc2 tree is log2(N)
6. Tree structure is good, it can make complexity from N to logN, only if the tree is well built, how to construct a fast tree is more difficult.
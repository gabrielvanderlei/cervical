
MPU_NUCA = [1, 0, 0]; % [ X, Y, Z]
MPU_CERVICAL = [0, 0, 1]; % [ X, Y, Z]

A = [1, 0, 0] % [X1, X2, X3] |
B = [0, 0, 0] % [Y1, Y2, Y3] -
C = [0, 0, 1] % [Z1, Z2, Z3] /

grid on
hold all
plot3(A,B,C)
view(-18,30)

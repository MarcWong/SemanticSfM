function [c,ceq] = constrains(Label1,x)
c = [];
ceq = [];
[n,m] = size(x);
for i = 1 : n
    ceq(i) =  x(i,13) - Label1(i);
end
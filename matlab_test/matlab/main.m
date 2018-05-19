%function [] = main(x0,P1,Label1)

x0 = [1,1,1,1,1,1,1,1,1,1,1,1,1];
P1 = [1,2];
Label1 = [0,1,1,1,1,0,1,0,1,1,1,1,1];

fun1 = @sumLeastSquare;
nonlcon = @constrains;

lb = [];
ub = [];

A = [];
b = [];
Aeq = [];
beq = [];


x = fmincon(@(x)fun1(P1,x),x0,A,b,Aeq,beq,lb,ub,@(x)nonlcon(Label1,x));
disp(x);

%end
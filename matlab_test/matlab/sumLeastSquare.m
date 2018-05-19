function Sum = sumLeastSquare(P1,x)
    % x n * 13
    % x(1-3):K    cam_K
    % x(4-6):R    cam_R
    % x(6-9):t     cam_t
    % x(10-12):X = (x,y,z)    pos_3dpoint
    
    % P1 = (u,v)  m_pos_2dpoint
    [n,m] = size(x);
    
    Sum = 0;
    for i = 1: n
        pos_proj = angelAxisRotatePoint(x(i,4:6),x(i,10:12));
        pos_proj = pos_proj + x(i,7:9);
        
        x_u= pos_proj(1)/pos_proj(3);
        y_u= pos_proj(2)/pos_proj(3);
        focal = x(i,1);
        principal_point_x = x(i,2);
        principal_point_y = x(i,3);
        
        protected_x  = principal_point_x + focal * x_u;
        protected_y  = principal_point_y + focal * y_u;
        
        out_residuals = (protected_x - P1(1)) * (protected_x - P1(1)) + (protected_y - P1(2))*(protected_y - P1(2));
        Sum = Sum + out_residuals;
    end
end
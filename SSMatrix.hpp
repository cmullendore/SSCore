//  SSMatrix.hpp
//  SSCore
//
//  Created by Tim DeBenedictis on 2/24/20.
//  Copyright © 2020 Southern Stars. All rights reserved.

#ifndef SSMatrix_hpp
#define SSMatrix_hpp

#include "SSVector.hpp"

class SSMatrix
{
    public:
    
    double m00, m01, m02;
    double m10, m11, m12;
    double m20, m21, m22;
    
    SSMatrix ( void );
    SSMatrix ( double m00, double m01, double m02, double m10, double m11, double m12, double m210, double m21, double m22 );
    
    SSMatrix transpose ( void );

    static SSMatrix identity ( void );
    static SSMatrix rotation ( int axis, double angle );
    
    SSVector multiply ( SSVector vec );
    SSMatrix multiply ( SSMatrix mat );
};

#endif /* SSMatrix_hpp */

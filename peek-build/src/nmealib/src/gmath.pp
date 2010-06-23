#line 1 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/gmath.c"








 

 

#line 1 "./include/nmea/gmath.h"








 




#line 1 "./include/nmea/info.h"








 

 




#line 1 "./include/nmea/time.h"








 

 




#line 1 "./include/nmea/config.h"








 


























#line 43 "./include/nmea/config.h"

#line 1 "/opt/TI/TMS470CGT4.6.1/include/assert.h"
 
 
 
 




#line 1 "/opt/TI/TMS470CGT4.6.1/include/linkage.h"
 
 
 
 




 





 
 
 
#line 27 "/opt/TI/TMS470CGT4.6.1/include/linkage.h"

#line 10 "/opt/TI/TMS470CGT4.6.1/include/assert.h"

 
 
 
 




 
 
 
#line 29 "/opt/TI/TMS470CGT4.6.1/include/assert.h"

#line 43 "/opt/TI/TMS470CGT4.6.1/include/assert.h"

 
extern  void _nassert(int);
extern  void _assert(int, const char *);

extern  void _abort_msg(const char *);




#line 68 "/opt/TI/TMS470CGT4.6.1/include/assert.h"







#line 83 "/opt/TI/TMS470CGT4.6.1/include/assert.h"

#line 46 "./include/nmea/config.h"





#line 17 "./include/nmea/time.h"








 
typedef struct _nmeaTIME
{
    int     year;        
    int     mon;         
    int     day;         
    int     hour;        
    int     min;         
    int     sec;         
    int     hsec;        

} nmeaTIME;



 
void nmea_time_now(nmeaTIME *t);





#line 17 "./include/nmea/info.h"























 
typedef struct _nmeaPOS
{
    double lat;          
    double lon;          

} nmeaPOS;





 
typedef struct _nmeaSATELLITE
{
    int     id;          
    int     in_use;      
    int     elv;         
    int     azimuth;     
    int     sig;         

} nmeaSATELLITE;





 
typedef struct _nmeaSATINFO
{
    int     inuse;       
    int     inview;      
    nmeaSATELLITE sat[(12)];  

} nmeaSATINFO;






 
typedef struct _nmeaINFO
{
    int     smask;       

    nmeaTIME utc;        

    int     sig;         
    int     fix;         

    double  PDOP;        
    double  HDOP;        
    double  VDOP;        

    double  lat;         
    double  lon;         
    double  elv;         
    double  speed;       
    double  direction;   
    double  declination;  

    nmeaSATINFO satinfo;  

} nmeaINFO;

void nmea_zero_INFO(nmeaINFO *info);





#line 15 "./include/nmea/gmath.h"

#line 24 "./include/nmea/gmath.h"







 

double nmea_degree2radian(double val);
double nmea_radian2degree(double val);



 

double nmea_ndeg2degree(double val);
double nmea_degree2ndeg(double val);

double nmea_ndeg2radian(double val);
double nmea_radian2ndeg(double val);



 

double nmea_calc_pdop(double hdop, double vdop);
double nmea_dop2meters(double dop);
double nmea_meters2dop(double meters);



 

void nmea_info2pos(const nmeaINFO *info, nmeaPOS *pos);
void nmea_pos2info(const nmeaPOS *pos, nmeaINFO *info);

double  nmea_distance(
        const nmeaPOS *from_pos,
        const nmeaPOS *to_pos
        );

double  nmea_distance_ellipsoid(
        const nmeaPOS *from_pos,
        const nmeaPOS *to_pos,
        double *from_azimuth,
        double *to_azimuth
        );

int     nmea_move_horz(
        const nmeaPOS *start_pos,
        nmeaPOS *end_pos,
        double azimuth,
        double distance
        );

int     nmea_move_horz_ellipsoid(
        const nmeaPOS *start_pos,
        nmeaPOS *end_pos,
        double azimuth,
        double distance,
        double *end_azimuth
        );





#line 14 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/gmath.c"

#line 1 "/opt/TI/TMS470CGT4.6.1/include/math.h"
 
 
 
 






 
 
 
 




 
 
 
#line 29 "/opt/TI/TMS470CGT4.6.1/include/math.h"


#line 1 "/opt/TI/TMS470CGT4.6.1/include/float.h"
 
 
 
 

 
 
 
 
 






#line 26 "/opt/TI/TMS470CGT4.6.1/include/float.h"

#line 36 "/opt/TI/TMS470CGT4.6.1/include/float.h"
 
#line 46 "/opt/TI/TMS470CGT4.6.1/include/float.h"

#line 32 "/opt/TI/TMS470CGT4.6.1/include/math.h"

#line 42 "/opt/TI/TMS470CGT4.6.1/include/math.h"

#line 50 "/opt/TI/TMS470CGT4.6.1/include/math.h"

 
 
 
        double modf(double x, double *y); 
        double asin(double x);
        double acos(double x);
        double atan(double x);
        double atan2(double y, double x);

        double ceil(double x);



        double cos(double x);
        double cosh(double x);
        double exp(double x);
        double fabs(double x);

        double floor(double x);



         double fmod(double x, double y);
         double frexp(double x, int *exp);
         double ldexp(double x, int exp);
         double log(double x);
         double log10(double x);
         double pow(double x, double y);
         double sin(double x);
         double sinh(double x);
         double tan(double x);
         double tanh(double x);
         double sqrt(double x);

#line 98 "/opt/TI/TMS470CGT4.6.1/include/math.h"

#line 115 "/opt/TI/TMS470CGT4.6.1/include/math.h"

 
 
 
#line 1 "/opt/TI/TMS470CGT4.6.1/include/cpp_inline_math.h"
 
 
 
  
#line 120 "/opt/TI/TMS470CGT4.6.1/include/math.h"


#line 16 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/gmath.c"



int isnan(double var)
{
    volatile double temp = var;
    return temp != temp;
}



 
double nmea_degree2radian(double val)
{ return (val * ((3.141592653589793) / 180)); }




 
double nmea_radian2degree(double val)
{ return (val / ((3.141592653589793) / 180)); }



 
double nmea_ndeg2degree(double val)
{
    double deg = ((int)(val / 100));
    val = deg + (val - deg * 100) / 60;
    return val;
}



 
double nmea_degree2ndeg(double val)
{
    double int_part;
    double fra_part;
    fra_part = modf(val, &int_part);
    val = int_part * 100 + fra_part * 60;
    return val;
}




 
double nmea_ndeg2radian(double val)
{ return nmea_degree2radian(nmea_ndeg2degree(val)); }




 
double nmea_radian2ndeg(double val)
{ return nmea_degree2ndeg(nmea_radian2degree(val)); }



 
double nmea_calc_pdop(double hdop, double vdop)
{
    return sqrt(pow(hdop, 2) + pow(vdop, 2));
}

double nmea_dop2meters(double dop)
{ return (dop * (5)); }

double nmea_meters2dop(double meters)
{ return (meters / (5)); }




 
double nmea_distance(
        const nmeaPOS *from_pos,     
        const nmeaPOS *to_pos        
        )
{
    double dist = ((double)((6378) * 1000)) * acos(
        sin(to_pos->lat) * sin(from_pos->lat) +
        cos(to_pos->lat) * cos(from_pos->lat) * cos(to_pos->lon - from_pos->lon)
        );
    return dist;
}







 
double nmea_distance_ellipsoid(
        const nmeaPOS *from_pos,     
        const nmeaPOS *to_pos,       
        double *from_azimuth,        
        double *to_azimuth           
        )
{
     
    double f, a, b, sqr_a, sqr_b;
    double L, phi1, phi2, U1, U2, sin_U1, sin_U2, cos_U1, cos_U2;
    double sigma, sin_sigma, cos_sigma, cos_2_sigmam, sqr_cos_2_sigmam, sqr_cos_alpha, lambda, sin_lambda, cos_lambda, delta_lambda;
    int remaining_steps; 
    double sqr_u, A, B, delta_sigma;

     
     _assert((from_pos != 0) != 0, "Assertion failed, (" "from_pos != 0" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/gmath.c" ", line " "120" "\n");
     _assert((to_pos != 0) != 0, "Assertion failed, (" "to_pos != 0" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/gmath.c" ", line " "121" "\n");

    if ((from_pos->lat == to_pos->lat) && (from_pos->lon == to_pos->lon))
    {  
        if ( from_azimuth != 0 )
            *from_azimuth = 0;
        if ( to_azimuth != 0 )
            *to_azimuth = 0;
        return 0;    
    }  

     
    f = (1 / 298.257223563);
    a = (6378137.0);
    b = (1 - f) * a;
    sqr_a = a * a;
    sqr_b = b * b;

     
    L = to_pos->lon - from_pos->lon;
    phi1 = from_pos->lat;
    phi2 = to_pos->lat;
    U1 = atan((1 - f) * tan(phi1));
    U2 = atan((1 - f) * tan(phi2));
    sin_U1 = sin(U1);
    sin_U2 = sin(U2);
    cos_U1 = cos(U1);
    cos_U2 = cos(U2);

     
    sigma = 0;
    sin_sigma = sin(sigma);
    cos_sigma = cos(sigma);
    cos_2_sigmam = 0;
    sqr_cos_2_sigmam = cos_2_sigmam * cos_2_sigmam;
    sqr_cos_alpha = 0;
    lambda = L;
    sin_lambda = sin(lambda);                            
    cos_lambda = cos(lambda);                       
    delta_lambda = lambda;
    remaining_steps = 20; 

    while ((delta_lambda > 1e-12) && (remaining_steps > 0)) 
    {  
         
        double tmp1, tmp2, tan_sigma, sin_alpha, cos_alpha, C, lambda_prev; 

         
        tmp1 = cos_U2 * sin_lambda;
        tmp2 = cos_U1 * sin_U2 - sin_U1 * cos_U2 * cos_lambda;  
        sin_sigma = sqrt(tmp1 * tmp1 + tmp2 * tmp2);                
        cos_sigma = sin_U1 * sin_U2 + cos_U1 * cos_U2 * cos_lambda;   
        tan_sigma = sin_sigma / cos_sigma;                  
        sin_alpha = cos_U1 * cos_U2 * sin_lambda / sin_sigma;  
        cos_alpha = cos(asin(sin_alpha));                 
        sqr_cos_alpha = cos_alpha * cos_alpha;                     
        cos_2_sigmam = cos_sigma - 2 * sin_U1 * sin_U2 / sqr_cos_alpha;
        sqr_cos_2_sigmam = cos_2_sigmam * cos_2_sigmam; 
        C = f / 16 * sqr_cos_alpha * (4 + f * (4 - 3 * sqr_cos_alpha));
        lambda_prev = lambda; 
        sigma = asin(sin_sigma); 
        lambda = L + 
            (1 - C) * f * sin_alpha
            * (sigma + C * sin_sigma * (cos_2_sigmam + C * cos_sigma * (-1 + 2 * sqr_cos_2_sigmam)));                                                
        delta_lambda = lambda_prev - lambda; 
        if ( delta_lambda < 0 ) delta_lambda = -delta_lambda; 
        sin_lambda = sin(lambda);
        cos_lambda = cos(lambda);
        remaining_steps--; 
    }   

     
    sqr_u = sqr_cos_alpha * (sqr_a - sqr_b) / sqr_b; 
    A = 1 + sqr_u / 16384 * (4096 + sqr_u * (-768 + sqr_u * (320 - 175 * sqr_u)));
    B = sqr_u / 1024 * (256 + sqr_u * (-128 + sqr_u * (74 - 47 * sqr_u)));
    delta_sigma = B * sin_sigma * ( 
        cos_2_sigmam + B / 4 * ( 
        cos_sigma * (-1 + 2 * sqr_cos_2_sigmam) -
        B / 6 * cos_2_sigmam * (-3 + 4 * sin_sigma * sin_sigma) * (-3 + 4 * sqr_cos_2_sigmam)
        ));

     
    if ( from_azimuth != 0 )
    {
        double tan_alpha_1 = cos_U2 * sin_lambda / (cos_U1 * sin_U2 - sin_U1 * cos_U2 * cos_lambda);
        *from_azimuth = atan(tan_alpha_1);
    }
    if ( to_azimuth != 0 )
    {
        double tan_alpha_2 = cos_U1 * sin_lambda / (-sin_U1 * cos_U2 + cos_U1 * sin_U2 * cos_lambda);
        *to_azimuth = atan(tan_alpha_2);
    }

    return b * A * (sigma - delta_sigma);
}



 
int nmea_move_horz(
    const nmeaPOS *start_pos,    
    nmeaPOS *end_pos,            
    double azimuth,              
    double distance              
    )
{
    nmeaPOS p1 = *start_pos;
    int RetVal = 1;

    distance /= (6378);  
    azimuth = nmea_degree2radian(azimuth);

    end_pos->lat = asin(
        sin(p1.lat) * cos(distance) + cos(p1.lat) * sin(distance) * cos(azimuth));
    end_pos->lon = p1.lon + atan2(
        sin(azimuth) * sin(distance) * cos(p1.lat), cos(distance) - sin(p1.lat) * sin(end_pos->lat));

    if(isnan(end_pos->lat) || isnan(end_pos->lon))
    {
        end_pos->lat = 0; end_pos->lon = 0;
        RetVal = 0;
    }

    return RetVal;
}






 
int nmea_move_horz_ellipsoid(
    const nmeaPOS *start_pos,    
    nmeaPOS *end_pos,            
    double azimuth,              
    double distance,             
    double *end_azimuth          
    )
{
     
    double f, a, b, sqr_a, sqr_b;
    double phi1, tan_U1, sin_U1, cos_U1, s, alpha1, sin_alpha1, cos_alpha1;
    double tan_sigma1, sigma1, sin_alpha, cos_alpha, sqr_cos_alpha, sqr_u, A, B;
    double sigma_initial, sigma, sigma_prev, sin_sigma, cos_sigma, cos_2_sigmam, sqr_cos_2_sigmam, delta_sigma;
    int remaining_steps;
    double tmp1, phi2, lambda, C, L;
    
     
     _assert((start_pos != 0) != 0, "Assertion failed, (" "start_pos != 0" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/gmath.c" ", line " "270" "\n");
     _assert((end_pos != 0) != 0, "Assertion failed, (" "end_pos != 0" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/gmath.c" ", line " "271" "\n");
    
    if (fabs(distance) < 1e-12)
    {  
        *end_pos = *start_pos;
        if ( end_azimuth != 0 ) *end_azimuth = azimuth;
        return ! (isnan(end_pos->lat) || isnan(end_pos->lon));
    }  

     
    f = (1 / 298.257223563);
    a = (6378137.0);
    b = (1 - f) * a;
    sqr_a = a * a;
    sqr_b = b * b;
    
     
    phi1 = start_pos->lat;
    tan_U1 = (1 - f) * tan(phi1);
    cos_U1 = 1 / sqrt(1 + tan_U1 * tan_U1);
    sin_U1 = tan_U1 * cos_U1;
    s = distance;
    alpha1 = azimuth;
    sin_alpha1 = sin(alpha1);
    cos_alpha1 = cos(alpha1);
    tan_sigma1 = tan_U1 / cos_alpha1;
    sigma1 = atan2(tan_U1, cos_alpha1);
    sin_alpha = cos_U1 * sin_alpha1;
    sqr_cos_alpha = 1 - sin_alpha * sin_alpha;
    cos_alpha = sqrt(sqr_cos_alpha);
    sqr_u = sqr_cos_alpha * (sqr_a - sqr_b) / sqr_b; 
    A = 1 + sqr_u / 16384 * (4096 + sqr_u * (-768 + sqr_u * (320 - 175 * sqr_u)));
    B = sqr_u / 1024 * (256 + sqr_u * (-128 + sqr_u * (74 - 47 * sqr_u)));
    
     
    sigma_initial = s / (b * A);
    sigma = sigma_initial;
    sin_sigma = sin(sigma);
    cos_sigma = cos(sigma);
    cos_2_sigmam = cos(2 * sigma1 + sigma);
    sqr_cos_2_sigmam = cos_2_sigmam * cos_2_sigmam;
    delta_sigma = 0;
    sigma_prev = 2 * (3.141592653589793);
    remaining_steps = 20;

    while ((fabs(sigma - sigma_prev) > 1e-12) && (remaining_steps > 0))
    {  
        cos_2_sigmam = cos(2 * sigma1 + sigma);
        sqr_cos_2_sigmam = cos_2_sigmam * cos_2_sigmam;
        sin_sigma = sin(sigma);
        cos_sigma = cos(sigma);
        delta_sigma = B * sin_sigma * ( 
             cos_2_sigmam + B / 4 * ( 
             cos_sigma * (-1 + 2 * sqr_cos_2_sigmam) - 
             B / 6 * cos_2_sigmam * (-3 + 4 * sin_sigma * sin_sigma) * (-3 + 4 * sqr_cos_2_sigmam)
             ));
        sigma_prev = sigma;
        sigma = sigma_initial + delta_sigma;
        remaining_steps --;
    }  
    
     
    tmp1 = (sin_U1 * sin_sigma - cos_U1 * cos_sigma * cos_alpha1);
    phi2 = atan2(
            sin_U1 * cos_sigma + cos_U1 * sin_sigma * cos_alpha1,
            (1 - f) * sqrt(sin_alpha * sin_alpha + tmp1 * tmp1)
            );
    lambda = atan2(
            sin_sigma * sin_alpha1,
            cos_U1 * cos_sigma - sin_U1 * sin_sigma * cos_alpha1
            );
    C = f / 16 * sqr_cos_alpha * (4 + f * (4 - 3 * sqr_cos_alpha));
    L = lambda -
        (1 - C) * f * sin_alpha * (
        sigma + C * sin_sigma *
        (cos_2_sigmam + C * cos_sigma * (-1 + 2 * sqr_cos_2_sigmam))
        );
    
     
    end_pos->lon = start_pos->lon + L;
    end_pos->lat = phi2;
    if ( end_azimuth != 0 )
    {
        *end_azimuth = atan2(
            sin_alpha, -sin_U1 * sin_sigma + cos_U1 * cos_sigma * cos_alpha1
            );
    }
    return ! (isnan(end_pos->lat) || isnan(end_pos->lon));
}



 
void nmea_info2pos(const nmeaINFO *info, nmeaPOS *pos)
{
    pos->lat = nmea_ndeg2radian(info->lat);
    pos->lon = nmea_ndeg2radian(info->lon);
}



 
void nmea_pos2info(const nmeaPOS *pos, nmeaINFO *info)
{
    info->lat = nmea_radian2ndeg(pos->lat);
    info->lon = nmea_radian2ndeg(pos->lon);
}

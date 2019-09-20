#ifndef CFFT1_H
#define CFFT1_H


#define      MAX_MATRIX_SIZE                   4194304             // 2048 * 2048
#define      PI                                           3.141592653
#define      MAX_VECTOR_LENGTH              10000             //

typedef struct Complex1
{
    double rl;
    double im;
}Complex1;

class CFFt1
{
public:
    CFFt1(void);
    ~CFFt1(void);

public:
    bool fft(Complex1 const inVec[], int const len, Complex1 outVec[]);            // 基于蝶形算法的快速傅里叶变换
    bool ifft(Complex1 const inVec[], int const len, Complex1 outVec[]);

    bool is_power_of_two(int num);
    int  get_computation_layers(int num);         // calculate the layers of computation needed for FFT
};

#endif // CFFT1_H

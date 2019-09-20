#include "cfft1.h"
#include "string.h"
#include "math.h"
#include <iostream>
CFFt1::CFFt1()
{

}

CFFt1::~CFFt1()
{

}

/**
// Fourier transform of 1 - dimension vector
// Param1: the input vector to be transformed
// Param 2: len of the input vector
// Param 3: output vector, which is the result of fft
*/
bool CFFt1::fft(const Complex1 inVec[], const int len, Complex1 outVec[])
{

    char msg[256] = "11111 ";

        if ((len <= 0) || (NULL == inVec) || (NULL == outVec))
        {
            return false;
            std::cout<<"len <= 0) || (NULL == inVec) || (NULL == outVec\n";
        }
        if (!is_power_of_two(len))
        {
            std::cout<<"len is not power_of_two\n";
            return false;
        }
        // create the weight array
        Complex1         *pVec = new Complex1[len];
        Complex1         *Weights = new Complex1[len];
        Complex1         *X = new Complex1[len];
        int                   *pnInvBits = new int[len];

        memcpy(pVec, inVec, len*sizeof(Complex1));

        // 计算权重序列
        double fixed_factor = (-2 * PI) / len;
        for (int i = 0; i < len / 2; i++) {
            double angle = i * fixed_factor;
            Weights[i].rl = cos(angle);
            Weights[i].im = sin(angle);
        }
        for (int i = len / 2; i < len; i++) {
            Weights[i].rl = -(Weights[i - len / 2].rl);
            Weights[i].im = -(Weights[i - len / 2].im);
        }

        int r = get_computation_layers(len);

        // 计算倒序位码
        int index = 0;
        for (int i = 0; i < len; i++) {
            index = 0;
            for (int m = r - 1; m >= 0; m--) {
                index += (1 && (i & (1 << m))) << (r - m - 1);
            }
            pnInvBits[i] = index;
            X[i].rl = pVec[pnInvBits[i]].rl;
            X[i].im = pVec[pnInvBits[i]].im;
        }

        // 计算快速傅里叶变换
        for (int L = 1; L <= r; L++) {
            int distance = 1 << (L - 1);
            int W = 1 << (r - L);

            int B = len >> L;
            int N = len / B;

            for (int b = 0; b < B; b++) {
                int mid = b*N;
                for (int n = 0; n < N / 2; n++) {
                    int index = n + mid;
                    int dist = index + distance;
                    pVec[index].rl = X[index].rl + (Weights[n*W].rl * X[dist].rl - Weights[n*W].im * X[dist].im);                      // Fe + W*Fo
                    pVec[index].im = X[index].im + Weights[n*W].im * X[dist].rl + Weights[n*W].rl * X[dist].im;
                }
                for (int n = N / 2; n < N; n++) {
                    int index = n + mid;
                    pVec[index].rl = X[index - distance].rl + Weights[n*W].rl * X[index].rl - Weights[n*W].im * X[index].im;        // Fe - W*Fo
                    pVec[index].im = X[index - distance].im + Weights[n*W].im * X[index].rl + Weights[n*W].rl * X[index].im;
                }
            }

            memcpy(X, pVec, len*sizeof(Complex1));
        }

        memcpy(outVec, pVec, len*sizeof(Complex1));
        if (Weights)      delete[] Weights;
        if (X)                 delete[] X;
        if (pnInvBits)    delete[] pnInvBits;
        if (pVec)           delete[] pVec;
        return true;
}

bool CFFt1::ifft(const Complex1 inVec[], const int len, Complex1 outVec[])
{
    char msg[256] = "11111 ";

        if ((len <= 0) || (!inVec))
            return false;
        if (false == is_power_of_two(len)) {
            return false;
        }

        double         *W_rl = new double[len];
        double         *W_im = new double[len];
        double         *X_rl = new double[len];
        double         *X_im = new double[len];
        double         *X2_rl = new double[len];
        double         *X2_im = new double[len];

        double fixed_factor = (-2 * PI) / len;
        for (int i = 0; i < len / 2; i++) {
            double angle = i * fixed_factor;
            W_rl[i] = cos(angle);
            W_im[i] = sin(angle);
        }
        for (int i = len / 2; i < len; i++) {
            W_rl[i] = -(W_rl[i - len / 2]);
            W_im[i] = -(W_im[i - len / 2]);
        }

        // 时域数据写入X1
        for (int i = 0; i < len; i++) {
            X_rl[i] = inVec[i].rl;
            X_im[i] = inVec[i].im;
        }
        memset(X2_rl, 0, sizeof(double)*len);
        memset(X2_im, 0, sizeof(double)*len);

        int r = get_computation_layers(len);
        if (-1 == r)
            return false;
        for (int L = r; L >= 1; L--) {
            int distance = 1 << (L - 1);
            int W = 1 << (r - L);

            int B = len >> L;
            int N = len / B;
            //sprintf(msg + 6, "B %d, N %d, W %d, distance %d, L %d", B, N, W, distance, L);
            //OutputDebugStringA(msg);

            for (int b = 0; b < B; b++) {
                for (int n = 0; n < N / 2; n++) {
                    int index = n + b*N;
                    X2_rl[index] = (X_rl[index] + X_rl[index + distance]) / 2;
                    X2_im[index] = (X_im[index] + X_im[index + distance]) / 2;
                    //sprintf(msg + 6, "%d, %d: %lf, %lf", n + 1, index, X2_rl[index], X2_im[index]);
                    //OutputDebugStringA(msg);
                }
                for (int n = N / 2; n < N; n++) {
                    int index = n + b*N;
                    X2_rl[index] = (X_rl[index] - X_rl[index - distance]) / 2;           // 需要再除以W_rl[n*W]
                    X2_im[index] = (X_im[index] - X_im[index - distance]) / 2;
                    double square = W_rl[n*W] * W_rl[n*W] + W_im[n*W] * W_im[n*W];          // c^2+d^2
                    double part1 = X2_rl[index] * W_rl[n*W] + X2_im[index] * W_im[n*W];         // a*c+b*d
                    double part2 = X2_im[index] * W_rl[n*W] - X2_rl[index] * W_im[n*W];          // b*c-a*d
                    if (square > 0) {
                        X2_rl[index] = part1 / square;
                        X2_im[index] = part2 / square;
                    }
                }
            }
            memcpy(X_rl, X2_rl, sizeof(double)*len);
            memcpy(X_im, X2_im, sizeof(double)*len);
        }

        // 位码倒序
        int index = 0;
        for (int i = 0; i < len; i++) {
            index = 0;
            for (int m = r - 1; m >= 0; m--) {
                index += (1 && (i & (1 << m))) << (r - m - 1);
            }
            outVec[i].rl = X_rl[index];
            outVec[i].im = X_im[index];
            //sprintf(msg + 6, "X_rl[i]: %lf, %lf,  index: %d", out_rl[i], out_im[i], index);
            //OutputDebugStringA(msg);
        }

        if (W_rl)      delete[] W_rl;
        if (W_im)    delete[] W_im;
        if (X_rl)      delete[] X_rl;
        if (X_im)     delete[] X_im;
        if (X2_rl)     delete[] X2_rl;
        if (X2_im)    delete[] X2_im;

        return true;
}

bool CFFt1::is_power_of_two(int num)
{
    int temp = num;
        int mod = 0;
        int result = 0;

        if (num < 2)
            return false;
        if (num == 2)
            return true;

        while (temp > 1)
        {
            result = temp / 2;
            mod = temp % 2;
            if (mod)
                return false;
            if (2 == result)
                return true;
            temp = result;
        }
        return false;
}

int CFFt1::get_computation_layers(int num)
{
    int nLayers = 0;
        int len = num;
        if (len == 2)
            return 1;
        while (true) {
            len = len / 2;
            nLayers++;
            if (len == 2)
                return nLayers + 1;
            if (len < 1)
                return -1;
        }
}

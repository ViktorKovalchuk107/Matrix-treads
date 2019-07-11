#include <cstdlib>
#include <iostream>
#include <random>
#include <ctime>
#include <memory>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <thread>
#include <mutex>
#include <future>


class Matrix {
public:
    explicit Matrix(size_t N) : N(N) {
        data = std::vector<double>(N*N);
    }

    Matrix(const Matrix &source) : Matrix(source.N) {
    }

    static Matrix Random (size_t N, double min, double max) {
        std::random_device rdev;
        std::mt19937 gen(rdev());
        gen.discard(N);

        std::uniform_real_distribution<double> rndgen(min, max);

        Matrix res(N);

        for(size_t i = 0; i < N*N; i++) {
            res.data[i] = rndgen(gen);
        }

        return res;
    }

    Matrix fastMult(const Matrix &b, size_t parts) const {
        Matrix res(N);

        size_t partSize = N/parts;

        std::vector<std::thread> th(parts);

        for(int i=0;i < parts - 1;i++) {
            th[i] = std::thread(blockMult, std::ref(*this), std::ref(b), i*partSize, (i+1)*partSize, std::ref(res));
        }

        th[parts - 1] = std::thread(blockMult, std::ref(*this), std::ref(b), (parts - 1)*partSize, N, std::ref(res));

        for(int i=0;i < parts;i++) {
            th[i].join();
        }

        return res;
    }

    Matrix operator *(const Matrix &b) const {
        Matrix res(N);
        for(size_t i = 0; i < N; i++) {
            for(size_t j = 0; j < N; j++) {
                double s = 0;
                for(size_t k = 0; k < N; k++) {
                    s += data[i*N + k] * b.data[k*N + j];
                }
                res.data[i*N + j] = s;
            }
        }

        return res;
    }

    bool operator ==(const Matrix &b) const {
        for(size_t i = 0; i < N*N; i++) {
            if(std::fabs(data[i] - b.data[i]) > std::numeric_limits<double>::epsilon()) return false;
        }
        return true;
    }

    bool operator !=(const Matrix &b) const {
        return !operator==(b);
    }

    friend std::ostream &operator<<(std::ostream &out, const Matrix & m) {
        for(size_t i = 0; i<m.N*m.N; i++) {
            out << std::fixed << std::setprecision(1) << m.data[(i / m.N)*m.N + i % m.N] << ((i+1) % m.N ? " " : "\n");
        }
        return out;
    }



private:
    size_t N;
    std::vector<double> data;

    void static blockMult(const Matrix &a, const Matrix &b, size_t start, size_t end, Matrix &res) {
        size_t N = a.N;

        for(size_t i = start; i < end; i++) {
            for(size_t j = 0; j < N; j++) {
                double s = 0;
                for(size_t k = 0; k < N; k++) {
                    s += a.data[i*N + k] * b.data[k*N + j];
                }
                res.data[i*N + j] = s;
            }
        }
    }
};

int main() {
    const size_t N = 800;
    const unsigned threads = 2;

    Matrix a = Matrix::Random(N, 0, 10);
    Matrix b = Matrix::Random(N, 0, 10);


    std::chrono::high_resolution_clock timer;

    auto start = timer.now();
    Matrix c = a * b;
    auto end = timer.now();
    std::cout << "Plain mult: " << std::chrono::duration<double>(end - start).count() <<std::endl;

    start = timer.now();
    Matrix d = a.fastMult(b, threads);
    end = timer.now();

    std::cout << "Multithreaded mult: " << std::chrono::duration<double>(end - start).count() <<std::endl;

    if(c != d) {
        std::cout <<std::endl << "Fast mult failed!!!" <<std::endl;
    }
    else {

    }



    return 0;
}
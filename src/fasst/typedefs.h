#include <Eigen/Core>
#include <vector>

typedef Eigen::Array<Eigen::MatrixXd, Eigen::Dynamic, 1> VectorMatrixXd;
typedef Eigen::Array<Eigen::MatrixXcd, Eigen::Dynamic, 1> VectorMatrixXcd;

typedef Eigen::Array<Eigen::MatrixXd, Eigen::Dynamic, Eigen::Dynamic>
    ArrayMatrixXd;
typedef Eigen::Array<Eigen::MatrixXcd, Eigen::Dynamic, Eigen::Dynamic>
    ArrayMatrixXcd;

typedef Eigen::Array<Eigen::VectorXd, Eigen::Dynamic, Eigen::Dynamic>
    ArrayVectorXd;
typedef Eigen::Array<Eigen::VectorXcd, Eigen::Dynamic, Eigen::Dynamic>
    ArrayVectorXcd;

typedef std::vector<VectorMatrixXcd> VectorVectorMatrixXcd;

typedef Eigen::DiagonalMatrix<double, Eigen::Dynamic> DiagonalMatrixXd;

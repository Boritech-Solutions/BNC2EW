#ifndef BNCSSLCONFIG_H
#define BNCSSLCONFIG_H

#include <QtNetwork>

// Singleton Class
// ---------------
class bncSslConfig : public QSslConfiguration {
 public:
  static bncSslConfig& Instance() {
    static bncSslConfig _sslConfig;
    return _sslConfig;
  }
 private:
  bncSslConfig();
  ~bncSslConfig();
};

#endif

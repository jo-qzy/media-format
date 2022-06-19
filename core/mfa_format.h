//
// Created by quzhenyu on 2022/6/18.
//

#ifndef MFA_FORMAT_H
#define MFA_FORMAT_H

class MfaFormat
{
public:
    MfaFormat();
    virtual ~MfaFormat();

    int WriteHeader();
    int WritePacket();
protected:

};

#endif // MFA_FORMAT_H

/**
 * File: FBrisk.cpp
 * Date: November 2017
 * Author: Andrew Kramer
 * Description: functions for BRISK descriptors
 * License: see the LICENSE.txt file
 *
 */
 
#include <vector>
#include <string>
#include <sstream>

#include <DVision/DVision.h>
#include "FBrisk.h"

using namespace std;

namespace DBoW2 {

// --------------------------------------------------------------------------

void FBrisk::meanValue(const std::vector<FBrisk::pDescriptor> &descriptors, 
  FBrisk::TDescriptor &mean)
{
  // if the vector of descriptors is empty release mean descriptor
  // release decrements the counter of elements in mean
  // if the counter reaches 0 mean is deallocated
  if (descriptors.empty())
  {
    mean.release();
    return;
  }
  
  // if there is only one element in the descriptor vector
  // set mean to be a deep copy of that descriptor
  else if (descriptors.size() == 1)
  {
    mean = descriptors[0]->clone();
  }

  // calculate mean of all given descriptors 
  else
  {
    vector<int> sum(FBrisk::L * 8, 0); // stores counts of set bits in all 
                                       // of the given descriptors
    // iterate over each descriptor
    for (size_t i = 0; i < descriptors.size(); ++i)
    {
      // create a pointer to unsigned char that points to the current descriptor
      // allows us to iterate over the descriptor byte-by-byte
      const cv::Mat &d = *descriptors[i];
      const unsigned char *p = d.ptr<unsigned char>();

      // iterate over each byte in the current descriptor
      for (int j = 0; j < d.cols; ++j, ++p)
      {
        // if a bit is set in the current byte increment the 
        // corresponding counter in sum
        if (*p & (1 << 7)) ++sum[j * 8];
        if (*p & (1 << 6)) ++sum[j * 8 + 1];
        if (*p & (1 << 5)) ++sum[j * 8 + 2];
        if (*p & (1 << 4)) ++sum[j * 8 + 3];
        if (*p & (1 << 3)) ++sum[j * 8 + 4];
        if (*p & (1 << 2)) ++sum[j * 8 + 5];
        if (*p & (1 << 1)) ++sum[j * 8 + 6];
        if (*p & 1)        ++sum[j * 8 + 7];
      }
    }

    // assigns mean to 1 by FBrisk::L vector of bytes
    mean = cv::Mat::zeros(1, FBrisk::L, CV_8U);
    // pointer to mean, sees mean as a vector of bytes
    unsigned char *p = mean.ptr<unsigned char>();

    // if a bit is set in half (or more) of the descriptors then it
    // is set in the mean
    const int N2 = (int)descriptors.size() / 2 + descriptors.size() % 2;
    for (size_t i = 0; i < sum.size(); ++i)
    {
      // set bit in mean if corresponding entry in sum is greater
      // than or equal to N2
      if (sum[i] >= N2)
      {
        *p |= 1 << (7 - (i % 8));
      }
      // advance pointer if the end of a byte has been reached
      if (i % 8 == 7) ++p;
    }
  }
}

// --------------------------------------------------------------------------
  
double FBrisk::distance(const FBrisk::TDescriptor &a, 
  const FBrisk::TDescriptor &b)
{
  // calculate hamming distance between the two given descriptors

  // assign pointers to char to each descriptor so they can be 
  // iterated over byte-by-byte
  const unsigned char *pa, *pb;
  pa = a.ptr<unsigned char>();
  pb = b.ptr<unsigned char>();

  int dist = 0;
  for (int i = 0; i < a.cols; ++i, ++pa, ++pb)
  {
    unsigned char thisByte = *pa ^ *pb;
    int byteCount;
    for (byteCount = 0; thisByte; byteCount++)
    {
      thisByte &= thisByte - 1;
    }
    dist += byteCount;
  }
  return dist;
}

// --------------------------------------------------------------------------
  
std::string FBrisk::toString(const FBrisk::TDescriptor &a)
{
  stringstream ss;
  const unsigned char *p = a.ptr<unsigned char>();

  for (int i = 0; i < a.cols; ++i, ++p)
  {
    ss << (int) *p << " ";
  }

  return ss.str();
}

// --------------------------------------------------------------------------
  
void FBrisk::fromString(FBrisk::TDescriptor &a, const std::string &s)
{
  a.create(1, FBrisk::L, CV_8U);
  unsigned char *p = a.ptr<unsigned char>();

  stringstream ss(s);
  for (int i = 0; i < FBrisk::L; ++i, ++p)
  {
    int n;
    ss >> n;

    if (!ss.fail())
    {
      *p = (unsigned char)n;
    }
  }
}

// --------------------------------------------------------------------------

void FBrisk::toMat32F(const std::vector<TDescriptor> &descriptors, 
  cv::Mat &mat)
{
  if(descriptors.empty())
  {
    mat.release();
    return;
  }
  
  const size_t N = descriptors.size();
  float *p = mat.ptr<float>();

  for (size_t i = 0; i < N; ++i)
  {
    const int C = descriptors[i].cols;
    const unsigned char *desc = descriptors[i].ptr<unsigned char>();

    for (int j = 0; j < C; ++j, p += 8)
    {
      p[0] = (desc[j] & (1 << 7) ? 1 : 0);
      p[1] = (desc[j] & (1 << 6) ? 1 : 0);
      p[2] = (desc[j] & (1 << 5) ? 1 : 0);
      p[3] = (desc[j] & (1 << 4) ? 1 : 0);
      p[4] = (desc[j] & (1 << 3) ? 1 : 0);
      p[5] = (desc[j] & (1 << 2) ? 1 : 0);
      p[6] = (desc[j] & (1 << 1) ? 1 : 0);
      p[7] = desc[j] & 1;
    }
  }
}

// --------------------------------------------------------------------------

} // namespace DBoW2


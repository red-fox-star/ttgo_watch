#pragma once
#include <stdexcept>

template <typename datatype, int buffer_size>
class MovingAverage {
  public:
    datatype data[buffer_size] = {0};

    void insert(datatype value) {
      position ++;

      if (position >= buffer_size)
        position = 0;

      data[position] = value;
    };

    datatype average(int window_size = buffer_size) {
      datatype sum = 0;

      for (int i = window_size - 1; i >= 0; i --)
        sum += fetch(position - i);

      return sum / (datatype) window_size;
    }

    datatype min(int window_size = buffer_size) {
      datatype _min = fetch(position);

      for (int i = window_size - 1; i >= 0; i --) {
        datatype val = fetch(position - i);
        if (val < _min) _min = val;
      }

      return _min;
    }

    datatype max(int window_size = buffer_size) {
      datatype _max = fetch(position);

      for (int i = window_size - 1; i >= 0; i --) {
        datatype val = fetch(position - i);
        if (val > _max) _max = val;
      }

      return _max;
    }

  private:
    int position = -1;

    datatype fetch(int pos) {
      int actual_pos = pos;

      if (actual_pos < 0)
        actual_pos += buffer_size;

      if (pos >= buffer_size) {
        char message[255];

        snprintf(
            message, sizeof(message),
            "%i is out of bounds: [0, %i] (%i resolved to %i)",
            pos, buffer_size, pos, actual_pos
        );

        throw std::out_of_range(message);
      }

      return data[actual_pos];
    }
};

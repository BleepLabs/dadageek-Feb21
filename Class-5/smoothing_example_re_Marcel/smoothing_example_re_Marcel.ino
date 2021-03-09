unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
int raw_reading;
int smoothed_reading;
int mapped_output;

void setup() {
  // put your setup code here, to run once:
  analogReadResolution(10);
}

void loop() {
  current_time = millis();

  if (current_time - prev_time[1] > 100) { // chage frequence ever 200 milliseconds
    prev_time[1] = current_time;
    raw_reading = analogRead(A2);
    smoothed_reading = (0, 71, raw_reading);
    mapped_output = map(smoothed_reading, 20, 600, 0, 1000);
  }

  if (current_time - prev_time[2] > 50) { // chage frequence ever 200 milliseconds
    prev_time[2] = current_time;
    Serial.print(mapped_output);
    Serial.print(" ");
    Serial.println(smoothed_reading);
  }



}



//based on https://playground.arduino.cc/Main/DigitalSmooth/
// This function continuously samples an input and puts it in an array that is "samples" in length.
// This array has a new "raw_in" value added to it each time "smooth" is called and an old value is removed
// It throws out the top and bottom 15% of readings and averages the rest

#define maxarrays 3 //max number of different variables to smooth
#define maxsamples 1000 //max number of points to sample and 
//reduce these numbers to save RAM

unsigned int smoothArray[maxarrays][maxsamples];

// sel should be a unique number for each occurrence
// samples should be an odd number greater that 7. It's the length of the array. The larger the more smooth but less responsive
// raw_in is the input. positive numbers in and out only.

unsigned int smooth(byte sel, unsigned int samples, unsigned int raw_in) {
  int j, k, temp, top, bottom;
  long total;
  static int i[maxarrays];
  static int sorted[maxarrays][maxsamples];
  boolean done;

  i[sel] = (i[sel] + 1) % samples;    // increment counter and roll over if necessary. -  % (modulo operator) rolls over variable
  smoothArray[sel][i[sel]] = raw_in;                 // input new data into the oldest slot

  for (j = 0; j < samples; j++) { // transfer data array into anther array for sorting and averaging
    sorted[sel][j] = smoothArray[sel][j];
  }

  done = 0;                // flag to know when we're done sorting
  while (done != 1) {      // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (samples - 1); j++) {
      if (sorted[sel][j] > sorted[sel][j + 1]) {    // numbers are out of order - swap
        temp = sorted[sel][j + 1];
        sorted[sel] [j + 1] =  sorted[sel][j] ;
        sorted[sel] [j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((samples * 15)  / 100), 1);
  top = min((((samples * 85) / 100) + 1  ), (samples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j < top; j++) {
    total += sorted[sel][j];  // total remaining indices
    k++;
  }
  return total / k;    // divide by number of samples
}

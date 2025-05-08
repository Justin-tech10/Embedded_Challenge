#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>
#include <arduinoFFT.h>

#define SPEAKER_PIN 5
#define SAMPLE_RATE 50
#define FFT_SAMPLES 64
#define LOW_MVMT_THRESH 50

double vReal[FFT_SAMPLES];
double vImag[FFT_SAMPLES];
arduinoFFT FFT;

uint8_t leftPixels[]  = {5, 6, 7, 8, 9};
uint8_t rightPixels[] = {0, 1, 2, 3, 4};

bool leftActive = false;
bool rightActive = false;

unsigned long previousBeepMillis = 0;
bool toneOn = false;
unsigned long beepInterval = 200;
int toneFrequency = 0;
int currentBrightness = 0;

void collectFFTData();
void analyzeFFT();
void handleOutput(double estimatedFreq);
int mapFloat(float x, float in_min, float in_max, int out_min, int out_max);


void collectFFTData() {
	Serial.println("Collecting FFT samples...");
	unsigned long lastTime = millis();
	int i = 0;
	
	while (i < FFT_SAMPLES) {
		if (millis() - lastTime >= 1000 / SAMPLE_RATE) {
			lastTime = millis();
			float x = CircuitPlayground.motionX();
			float y = CircuitPlayground.motionY();
			float z = CircuitPlayground.motionZ();
			vReal[i] = sqrt(x * x + y * y + z * z); // magnitude
			vImag[i] = 0;
			i++;
		}
	}
	
	// Remove DC offset
	double avg = 0;
	for (int i = 0; i < FFT_SAMPLES; i++) avg += vReal[i];
	avg /= FFT_SAMPLES;
	for (int i = 0; i < FFT_SAMPLES; i++) vReal[i] -= avg;
}

void analyzeFFT() {
	Serial.println("Analyzing FFT...");
	FFT.Windowing(vReal, FFT_SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
	Serial.println("Window done");
	FFT.Compute(vReal, vImag, FFT_SAMPLES, FFT_FORWARD);
	Serial.println("Compute done");
	FFT.ComplexToMagnitude(vReal, vImag, FFT_SAMPLES);
	Serial.println("Magnitude done");
	double peakFreq = FFT.MajorPeak(vReal, FFT_SAMPLES, SAMPLE_RATE);
	
	// Find max magnitude after FFT
	double maxMag = 0;
	for (int i = 0; i < FFT_SAMPLES / 2; i++) if (vReal[i] > maxMag) maxMag = vReal[i];
	
	Serial.print("Peak Frequency: ");
	Serial.print(peakFreq, 2);
	Serial.print(" Hz | Magnitude: ");
	Serial.println(maxMag, 2);
	Serial.print("FFT Peak Frequency: ");
	Serial.print(peakFreq, 2);
	Serial.println(" Hz");
	
	if (maxMag < LOW_MVMT_THRESH) {
		Serial.println("Low movement detected — ignoring FFT result.");
		handleOutput(0.0); // treat as stationary
		return;
	}
	handleOutput(peakFreq);
}

void handleOutput(double estimatedFreq) {
	auto& strip = CircuitPlayground.strip;
	currentBrightness = 0;
	toneFrequency = 0;
	
	// Tremor
	if (estimatedFreq >= 3.0 && estimatedFreq <= 5.0) {
		currentBrightness = mapFloat(estimatedFreq, 3.0, 5.0, 50, 200);
		toneFrequency = mapFloat(currentBrightness, 50, 200, 300, 800);
		for (uint8_t i : rightPixels) strip.setPixelColor(i, 0);
		for (uint8_t i : leftPixels)  strip.setPixelColor(i, strip.Color(currentBrightness, 0, 0)); // Red
		leftActive = true;
		rightActive = false;
		Serial.println("Tremor Detected (Red)");
	}
	
	// Dyskinesia
	else if (estimatedFreq > 5.0 && estimatedFreq <= 7.0) {
		currentBrightness = mapFloat(estimatedFreq, 5.0, 7.0, 50, 200);
		toneFrequency = mapFloat(currentBrightness, 50, 200, 300, 800);
		for (uint8_t i : leftPixels)  strip.setPixelColor(i, 0);
		for (uint8_t i : rightPixels) strip.setPixelColor(i, strip.Color(0, 0, currentBrightness)); // Blue
		leftActive = false;
		rightActive = true;
		Serial.println("Dyskinesia Detected (Blue)");
	}
	
	// Normal or Stillness movement
	else {
		for (int i = 0; i < 10; i++) {
			strip.setPixelColor(i, strip.Color(0, 100, 0)); // Green
		}
		strip.show();
		noTone(SPEAKER_PIN);
		leftActive = false;
		rightActive = false;
		Serial.println("Normal or No Movement Detected (Green)");
		return;
	}
	
	strip.show();
}

int mapFloat(float x, float in_min, float in_max, int out_min, int out_max) {
	return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void setup() {
	Serial.begin(9600);
	unsigned long waitStart = millis();
	while (!Serial && millis() - waitStart < 3000);
	Serial.println("Running headless.");


	CircuitPlayground.begin();
	CircuitPlayground.strip.setBrightness(35);
	CircuitPlayground.strip.setPixelColor(0, 255, 255, 255); // white flash
	CircuitPlayground.strip.show();
	delay(500);
	CircuitPlayground.strip.clear();
	CircuitPlayground.strip.show();
	Serial.println("FFT tremor detection system initialized.");
}

void loop() {
	collectFFTData();
	analyzeFFT();
	delay(100);

	if ((leftActive || rightActive) && millis() - previousBeepMillis >= beepInterval) {
		previousBeepMillis = millis();
		if (toneOn) noTone(SPEAKER_PIN);
		else tone(SPEAKER_PIN, toneFrequency);
		toneOn = !toneOn;
	}
}

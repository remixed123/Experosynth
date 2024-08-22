#include "daisy_patch_sm.h"
#include "daisysp.h"
//#include <stdio.h>

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;

DaisyPatchSM patch;
Oscillator   osc_a, osc_b, osc_c;
Switch toggle, button;
Adsr envelope;

uint32_t gatelenfreq[2] = {1000,1000}; //Gate length/frequency setup

void selectwaveform(int);

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    patch.ProcessAllControls();
    toggle.Debounce();
    button.Debounce();

    // Coase Tune
    float knob_coarse = patch.GetAdcValue(CV_1);
    float coarse_tune = fmap(knob_coarse, 12, 84);

    // CV Tune V/OCT
    float cv_voct = patch.GetAdcValue(CV_5);
    float voct    = fmap(cv_voct, 0, 60);

    // Gate Length (rising switch) - First Gate Length and Frequency (falling switch)
    float knob_gatetime = patch.GetAdcValue(CV_2);
    float gate_timing = fmap(knob_gatetime, 10, 10000, Mapping::EXP );

    // CV tweak of the above
    float cv_gatetime = patch.GetAdcValue(CV_7);
    float cv_gate_timing = fmap(cv_gatetime, 10, 2000, Mapping::EXP);

    gatelenfreq[0] = gate_timing + cv_gate_timing;

    // Gate Frequency (rising switch) - Second Gate Length and Frequency (falling switch)
    float knob_gatefrequency = patch.GetAdcValue(CV_4);
    float gate_frequency = fmap(knob_gatefrequency, 10, 10000, Mapping::EXP);

    // CV tweak of the above
    float cv_gatefrequency = patch.GetAdcValue(CV_8);
    float cv_gate_frequency = fmap(cv_gatefrequency, 10, 2000, Mapping::EXP);

    gatelenfreq[1] = gate_frequency + cv_gate_frequency;

    // Convert from MIDI note number to frequency */
    float midi_nn = fclamp(coarse_tune + voct, 0.f, 127.f);
    float freq_a  = mtof(midi_nn);

    // CV detune tweak
    float cv_detune = patch.GetAdcValue(CV_6);
    float cv_de_tune = fmap(cv_detune, 0, 0.5);

    // Calculate a detune amount 
    float detune_amt = patch.GetAdcValue(CV_3) + cv_de_tune;
    float freq_b     = freq_a + (0.05 * freq_a * detune_amt);
    float freq_c     = freq_a - (0.05 * freq_a * detune_amt);

    // Set all three oscillator frequencies
    osc_a.SetFreq(freq_a);
    osc_b.SetFreq(freq_b);
    osc_c.SetFreq(freq_c); 

    // Process each sample of the oscillator and send to audio out
    for(size_t i = 0; i < size; i++)
    {
        float sig = osc_a.Process() + osc_b.Process() + osc_c.Process(); 
        float sig2 = osc_a.Process(); 
        OUT_L[i]  = sig; // Triple oscillator out of OUT_L[i]
        OUT_R[i]  = sig2; // Single oscillator out of OUT_R[i]
    }
}

void EnvelopeCallback(uint16_t **output, size_t size)
{
    // Process the controls 
    patch.ProcessAnalogControls();

    // Set the input value of the ADSR
    bool env_state;
    if(patch.gate_in_1.State())
        env_state = true;
    else
        env_state = false;

    // Attack is time based, increase time for a slower attack
    envelope.SetAttackTime(0.07);

    // Decay is time based, adjust knob CV_2 to alter decay time
    float knob_decay = patch.GetAdcValue(CV_2);
    envelope.SetDecayTime(knob_decay);

    // Sustain is level base, alter level between 0 and 1
    envelope.SetSustainLevel(0.7);

    // Release is time based, adjust knob CV_4 to alter release time
    float knob_release = patch.GetAdcValue(CV_4);
    envelope.SetReleaseTime(knob_release);

    for(size_t i = 0; i < size; i++)
    {
        // convert to 12-bit integer (0-4095)
        uint16_t value = (envelope.Process(env_state) * 4095.0);
        output[0][i]   = value; // Envelope out to CV_4 - Jack
        output[1][i]   = value; // Envelope out to CV_OUT_2 - LED
    }
}

// Button is used to cycle through different oscillators - could setup to cycle through array, but this is clear and allows for easier tweaking such as combo of waveforms
void selectwaveform(int selected)
{
    if (selected == 1)
    {
        //patch.PrintLine("POLYBLEP SAW Selected\n");
        osc_a.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
        osc_b.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
        osc_c.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    }
    else if (selected == 2)
    {
        //patch.PrintLine("POLYBLEP SQUARE Selected\n");
        osc_a.SetWaveform(Oscillator::WAVE_POLYBLEP_SQUARE);
        osc_b.SetWaveform(Oscillator::WAVE_POLYBLEP_SQUARE);
        osc_c.SetWaveform(Oscillator::WAVE_POLYBLEP_SQUARE);
    }
    else if (selected == 3)
    {
        //patch.PrintLine("POLYBLEP TRI Selected\n");
        osc_a.SetWaveform(Oscillator::WAVE_POLYBLEP_TRI);
        osc_b.SetWaveform(Oscillator::WAVE_POLYBLEP_TRI);
        osc_c.SetWaveform(Oscillator::WAVE_POLYBLEP_TRI);
    }
    else if (selected == 4)
    {
        //patch.PrintLine("SIN Selected\n");
        osc_a.SetWaveform(Oscillator::WAVE_SIN);
        osc_b.SetWaveform(Oscillator::WAVE_SIN);
        osc_c.SetWaveform(Oscillator::WAVE_SIN);
    }
}


int main(void)
{
    patch.Init();
    //patch.StartLog(true);
    //patch.PrintLine("XXXXX MAIN STARTED XXXXX");

    System::Delay(500);

    toggle.Init(DaisyPatchSM::B8, patch.AudioCallbackRate());
    button.Init(DaisyPatchSM::B7, patch.AudioCallbackRate());

    // Initialize the ADSR
    envelope.Init(48000);

    osc_a.Init(patch.AudioSampleRate());
    osc_b.Init(patch.AudioSampleRate());
    osc_c.Init(patch.AudioSampleRate());

    osc_a.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    osc_b.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    osc_c.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);

    // Configure the DAC to generate audio-rate signals in a callback amd envelope
    patch.StartDac(EnvelopeCallback);
    patch.StartAudio(AudioCallback);

    uint32_t now, dact, usbt, gatetf, gatetf1, gatetf2;
    now = dact = usbt = System::GetNow();
    gatetf = gatetf1 = gatetf2 = now;

    bool memstate = false;
    bool buttonstate = false;
    int waveselect = 1;

    int rcount, fcount;
    rcount = 0;
    fcount = 0;

    dsy_gpio_write(&patch.gate_out_1,0);
    dsy_gpio_write(&patch.gate_out_2,1);

    int i = 0;

    while(1) {

        now = System::GetNow();
        buttonstate = button.Pressed();

        // Change the Oscillator waveform by pressing button
        if (memstate != buttonstate)
        {
            if (memstate == false)
            {
                waveselect++;
                selectwaveform(waveselect);
                //patch.PrintLine("memstate: %s  buttonstate: %s  waveselect %d\n", memstate? "True" : "False", buttonstate? "True" : "False", waveselect);
                if (waveselect == 4)
                    waveselect = 0;
             }
        }
        memstate = buttonstate; // Reset button state

        // Initialise gate_out_1 and gate_out_2 type dependent on switch being up or down
        if (toggle.RisingEdge() && rcount < 1) // Switch up
        {
            //patch.PrintLine("Rising Edge Occurred");
            if (dsy_gpio_read(&patch.gate_out_1) == dsy_gpio_read(&patch.gate_out_2))
            {
                dsy_gpio_write(&patch.gate_out_2,0);;
                dsy_gpio_write(&patch.gate_out_2,1);
            }
            rcount++;
            fcount=0;
        }
        if (toggle.FallingEdge() && fcount < 1) // Switch down
        {
            //patch.PrintLine("Falling Edge Occurred");
            dsy_gpio_write(&patch.gate_out_1,0);
            dsy_gpio_write(&patch.gate_out_2,0);
            fcount++;
            rcount=0;
        }

        // Select gate_out_1 and gate_out_2 type dependent on switch being up or down
        if (toggle.Pressed()) // Gate reverse option - CV_2 knob adjusts gate_out_1 length, CV_4 knob adjusts frequency of gate_out_1, gate_out_2 does the reverse
        {
            if(now - gatetf > gatelenfreq[i]) 
            {
                dsy_gpio_toggle(&patch.gate_out_1);
                dsy_gpio_toggle(&patch.gate_out_2);
                i++;
                gatetf = now;
                if (i > 1) {i=0;}
            }
        }
        else // Gate individual option - CV_2 knob adjusts gate_out_1 length and frequency, CV_4 knob adjusts gate_out_2 length and frequency
        {
            if(now - gatetf1 > gatelenfreq[0]) 
            {
                dsy_gpio_toggle(&patch.gate_out_1);
                gatetf1 = now;
            }
            if(now - gatetf2 > gatelenfreq[1]) 
            {
                dsy_gpio_toggle(&patch.gate_out_2);
                gatetf2 = now;
            }

        }
    }
}

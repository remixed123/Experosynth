# Experosynth

Experosynth is a synthesizer that has been developed for the ElectroSmith Patch.Init() Eurorack module. As the name suggests, Experosynth has
been designed for experimental synthesis, with interesting rythmic patterns and sound effects possible through tweaking knobs and CV inputs.

Experosynth includes 1 triple VCO, 1 single VCO, 2 gate outs and 1 gate in and an envelope out, with much if this tweakable by knobs and
CV inputs. The gate outs can function in tandam (gate reverse option) allowing for interesting effects when used with other modular gear. The
envelope can also be used to produce interesting effects.

Experosynth does not require a sequencer, but one can be used if you require syncing with other gear, however this does take away from some of
the experimental nature of this synth.

## Gate Modes

Experosynth has 2 modes which work with the gate outs.

* Gate reverse mode - B8 Switch Up - CV_2 knob adjusts gate_out_1 length, CV_4 knob adjusts frequency of gate_out_1, gate_out_2 does the reverse of gate_out_1.
* Gate individual mode - B8 Switch Down - CV_2 knob adjusts gate_out_1 length and frequency, CV_4 knob adjusts gate_out_2 length and frequency.

## Controls

* CV_1 Knob - Tunes the base frequency of all three oscillators and the single oscillator.
* CV_3 Knob - Adjusts the amount of detuning between all tthe three voices of the triple oscillator.
* CV_2 Knob - See gate modes above. CV_2 also adjusts Decay of the Envelope.
* CV_4 Knob - See gate modes above. CV_4 also adjusts Release of the Envelope.
* B8 Switch - Changes gate mode, see gate modes above.
* B7 Button - Loops through oscillator waveform - Saw, Square, Triangle, Sine.
* B10 Jack (gate_in_1) - Gate in for the envelope.
* C10 Jack (CV_OUT_1) - Envelope out.
* C1 LED (CV_OUT_2) - LED displays the envelope.
* B9 Jack (gate_in_2) - Reserved for future use.
* B5 Jack (gate_out_1) - Gate out 1, see gate modes above.
* B6 Jack (gate_out_2) - Gate out 2, see gate modes above.
* CV_5 Jack - V/Oct in for the VCOs.
* CV_6 Jack - Detune for triple VCO.
* CV_7 Jack - CV control over CV_2 knob, see gate modes above.
* CV_8 Jack - CV control over CV_4 knob, see gate modes above.
* IN_L(i) - Not in use, suggestions welcome.
* IN_R(i) - Not in use, suggestions welcome.
* OUT_L(i) - Triple VCO out.
* OUT_R(i) - Single VCO out.

## Suggested Patching

As with any modular synth, you can get very creative, especially when using with other moduler gear. Utilising the gates and envelope of the Experosynth
can result in some nice effects, whether you are using the Experosynth by its self or with external modular gear.

* Gate_out_1 into Gate_in_1, CV_4 (Env) into CV_5 (Tune), Out_L[i] to Mix (Recommended to output to a Filter and modulate with CV_4 or an LFO), adjust CV_2 and CV_4 knobs to your liking
* Use the Gate_out_1 and Gate_out_2 to trigger external envelopes that are then used with VCFs or VCAs, the bouncing between the synced gates, especially in Gate Reverse Mode can produce interesting effects.
* Patch an LFO into CV_7 and/or CV_8 can create interesting speeding up and slowing down effects.

## Potential Future Updates

* Adding external syncing capabilities that will allow for syncing while ensuring the gate and envelope features remain intact. Reserving B9 for this purposes.
* What should I do with the audio ins? Suggestions welcome.

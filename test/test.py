import soundcoder

stream = soundcoder.wav_to_mp3(open('sound.wav'))
output = open('test.mp3', 'wb')
output.writelines(stream)
output.flush()
output.close()

stream = soundcoder.wav_to_aac(open('sound.wav'))
output = open('test.mp4', 'wb')
output.writelines(stream)
output.flush()
output.close()

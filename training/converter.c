#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <byteswap.h>
#include <lame/lame.h>

#define         MAX_U_32_NUM            0xFFFFFFFF

typedef struct {
  uint32_t chunkId; //chunkID big
  uint32_t chunkSize; // chunkSize little
  uint32_t format; //format big
  uint32_t subChunk1Id; //subshunk1 id big
  uint32_t subChunk1Size; // subshunk1 size little
  uint16_t audioFormat; //audioFormat little
  uint16_t numChannels; //NumChannles little
  uint32_t sampleRatio; //sampleRatio little
  uint32_t byteRate; //byteRate little
  uint16_t blockAlign; //BlockAlign little
  uint16_t bitPerSample; //BitsPerSample little
  uint32_t subChunk2Id; //subchunk2id big
  uint32_t subChunk2Size; //subchunk2size little
} __attribute__((__packed__)) header_t;

header_t header;

int swap(header_t *header)
{
  header->chunkId = __bswap_32(header->chunkId);
  header->format = __bswap_32(header->format);
  header->subChunk1Id = __bswap_32(header->subChunk1Id);
  header->subChunk2Id = __bswap_32(header->subChunk2Id);

  return 0;
}

int encode_to_file(lame_global_flags *gfp, const short *leftPcm, const short *rightPcm, FILE *out)
{
  int numSamples = header.subChunk2Size / header.blockAlign;
  int mp3BufferSize = numSamples * 5 / 4 + 7200; // worst case estimate
  unsigned char mp3Buffer[mp3BufferSize];

  int mp3size = lame_encode_buffer(gfp, leftPcm, rightPcm, numSamples, mp3Buffer, mp3BufferSize);
  if (!(mp3size > 0)) {
    printf("No data was encoded by lame_encode_buffer. Return code:%d \n", mp3size);
    return -1;
  }

  fwrite((void *)mp3Buffer, sizeof(unsigned char), mp3size, out);

  int flushSize = lame_encode_flush(gfp, mp3Buffer, mp3BufferSize);

  fwrite((void *)mp3Buffer, sizeof(unsigned char), flushSize, out);

  lame_mp3_tags_fid(gfp, out);

  printf("Wrote %d bytes\n", mp3size + flushSize);

  return 0;
}

int main(int argc, char *argv[])
{
  FILE *fp;
  uint16_t skip;
  lame_global_flags *gf = lame_init();

  fp = fopen("onion.wav", "rb");

  if (!fp) {
    return -1;
  }

  fread(&header, 1, sizeof(header), fp);

  swap(&header);

  if (header.subChunk2Id != 0x64617461) {
    int i;
    for (i = 0; i < 10; i++) {
      printf("%d: %x\n", i, skip);
      fread(&skip, 1, sizeof(skip), fp);
      if (skip != 0x6164) {
        continue;
      }
      printf("aui\n");
      fread(&skip, 1, sizeof(skip), fp);
      if (skip != 0x6174) {
        continue;
      }

      header.subChunk2Id = 64617461;
      fread(&header.subChunk2Size, 1, sizeof(header.subChunk2Size), fp);
      break;

      printf("%d: %x\n", i, skip);
    }
  }




  printf("chunkid: %x\n", header.chunkId);
  printf("chunksize: %x\n", header.chunkSize);
  printf("format: %x \n", header.format);
  printf("subchunk1id: %x\n", header.subChunk1Id);
  printf("subshunk2size: %x\n", header.subChunk1Size);
  printf("audioformat: %x \n", header.audioFormat);
  printf("numchanel: %x \n", header.numChannels);
  printf("sampleratio: %x\n", header.sampleRatio);
  printf("byteraitio: %x \n", header.byteRate);
  printf("blockalig: %x \n", header.blockAlign);
  printf("bitpersample: %x\n", header.bitPerSample);
  printf("subchunk2id: %x \n", header.subChunk2Id);
  printf("subshunk2size: %x \n", header.subChunk2Size);
  printf("\n");

  int idx;
  int numSamples = header.subChunk2Size / header.blockAlign;
  FILE *outf = fopen("onion.mp3", "w+b");

  /* set the defaults from info incase we cannot determine them from file */
//  lame_set_num_samples(gf, MAX_U_32_NUM);

//  lame_set_write_id3tag_automatic(gf, 0);

  lame_set_brate(gf, 192); // increase bitrate
  lame_set_quality(gf, 3);
  lame_set_bWriteVbrTag(gf, 0);

  short *leftPcm = malloc(header.subChunk2Size / header.numChannels * sizeof(short));
  short *rightPcm = malloc(header.subChunk2Size / header.numChannels * sizeof(short));

  for (idx = 0; idx < numSamples; idx++) {
    fread(&leftPcm[idx], 1, 2, fp);
    fread(&rightPcm[idx], 1, 2, fp);
  }
  fclose(fp);

  lame_set_num_channels(gf, header.numChannels);
  lame_set_num_samples(gf, header.subChunk2Size / header.blockAlign);
//  lame_set_in_samplerate(gf, header.sampleRatio);
//  lame_set_num_samples(gf, header.subChunk2Size / (header.numChannels * ((header.bitPerSample + 7) / 8)));

  if (lame_init_params(gf) != 0) {
    printf("error init params\n");
    return -1;
  }

  encode_to_file(gf, leftPcm, rightPcm, outf);

  fclose(outf);

  lame_close(gf);

  free(leftPcm);
  free(rightPcm);
  return 0;
}

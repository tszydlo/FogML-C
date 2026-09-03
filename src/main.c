/*
   Copyright 2026 FogML
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "fogml_sdk/fogml.h"
#include "fogml_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 4096
#define MAX_CHANNEL_LEN 32

/* Feature columns of a dataset CSV row, in the order they appear after the
   7 metadata columns (segment,anomaly,train,channel,sampling,duration,len). */
static const char *const feature_names[] = {
    "mean",
    "var",
    "std",
    "kurtosis",
    "skew",
    "n_peaks",
    "smooth10_n_peaks",
    "smooth20_n_peaks",
    "diff_peaks",
    "diff2_peaks",
    "diff_var",
    "diff2_var",
    "gaps_squared",
    "len_weighted",
    "var_div_duration",
    "var_div_len"
};

#define FOGML_FEATURE_COUNT (sizeof(feature_names) / sizeof(feature_names[0]))

typedef struct {
    int segment;
    int anomaly;
    int train;
    char channel[MAX_CHANNEL_LEN];
    int sampling;
    int duration;
    int len;
} segment_metadata_t;

/* Splits a CSV row (already stripped of its trailing newline) into the 7
   metadata fields and the remaining FOGML_FEATURE_COUNT feature values. */
static void parse_csv_line(char *line, segment_metadata_t *meta, float *features) {
    char *field = strtok(line, ",");
    int idx = 0;

    while (field != NULL) {
        switch (idx) {
            case 0: meta->segment = atoi(field); break;
            case 1: meta->anomaly = atoi(field); break;
            case 2: meta->train = atoi(field); break;
            case 3:
                strncpy(meta->channel, field, MAX_CHANNEL_LEN - 1);
                meta->channel[MAX_CHANNEL_LEN - 1] = '\0';
                break;
            case 4: meta->sampling = atoi(field); break;
            case 5: meta->duration = atoi(field); break;
            case 6: meta->len = atoi(field); break;
            default:
                if ((size_t)(idx - 7) < FOGML_FEATURE_COUNT) {
                    features[idx - 7] = strtof(field, NULL);
                }
                break;
        }

        idx++;
        field = strtok(NULL, ",");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <channel.csv>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error: could not open file '%s'\n", argv[1]);
        return 1;
    }

    char line[MAX_LINE_LEN];

    /* first line is the legend, skip it */
    if (fgets(line, sizeof(line), fp) == NULL) {
        printf("Error: '%s' is empty\n", argv[1]);
        fclose(fp);
        return 1;
    }

    int total_lines = 0;
    int train_lines = 0;
    int matched_count = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        segment_metadata_t meta;
        float features[FOGML_FEATURE_COUNT];

        parse_csv_line(line, &meta, features);

        printf("segment=%d anomaly=%d train=%d channel=%s sampling=%d duration=%d len=%d\n",
               meta.segment, meta.anomaly, meta.train, meta.channel,
               meta.sampling, meta.duration, meta.len);


        /*FogML Learning - only for rows belonging to the training split, and only normal (good) data for training */
        if (meta.train && !meta.anomaly) {        
            fogml_learning(features);
        }

        /* FogML Processing - only for rows not belonging to the training split */
        if (!meta.train) {
            float score;
            fogml_processing(features, &score);
            printf("score=%f\n", score);

            // Only LOF score > 1.5 trated as anomaly - threshold can be set in the semi-supervised learning
            int detected_anomaly = (score > 1.5) ? 1 : 0;

            if (detected_anomaly == meta.anomaly) {
                matched_count++;
            }
        }

        total_lines++;
        if (meta.train) {
            train_lines++;
        }
    }

    fclose(fp);

    printf("Train lines: %d / %d\n", train_lines, total_lines);

    printf("Matched count: %d \n", matched_count);

    if (train_lines > 0) {
        //float accuracy = matched_count / (float)train_lines;//((double)total_lines - train_lines);
        float accuracy = matched_count / ((double)total_lines - train_lines);
        printf("Accuracy: %f\n", accuracy);
    }

    return 0;
}

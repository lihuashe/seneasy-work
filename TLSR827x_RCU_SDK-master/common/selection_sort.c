/******************************************************************************
 * @file     selection_sort.c
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/

void selection_sort_char(unsigned char * arr, int size){
    //sorting
    int i, j;
    for(i = 0; i < size; ++i)
    {
        for( j = i + 1; j < size; ++j)
        {
            if (arr[j] < arr[i])
            {
                unsigned char t = arr[i];
                arr[i] = arr[j];
                arr[j] = t;
            }
        }
    }
}

void selection_sort_int(unsigned int * arr, int size){
    //sorting
    int i, j;
    for(i = 0; i < size; ++i)
    {
        for( j = i + 1; j < size; ++j)
        {
            if (arr[j] < arr[i])
            {
                unsigned int t = arr[i];
                arr[i] = arr[j];
                arr[j] = t;
            }
        }
    }
}



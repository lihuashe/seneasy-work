/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ft3308_handle.h
* @brief
* @details
* @author   huzhuohuan
* @date     2024-08-27
* @version  v1.0
*********************************************************************************************************
*/

#include "ft3308_handle.h"
#if (TOUCH_ENABLE)
Seat_store seat_st;
Func_data func_data = FUNC_DATA_INIT;
Shape_ret_data Shape_data = SHAPE_RET_DATA_INIT;

/**
 * @brief Calculates the approximate angle in degrees between two points.
 *
 * This function uses a lookup table to quickly estimate the angle (in degrees)
 * from the origin to the point (x, y) without using floating-point operations.
 * It's a fast approximation suitable for embedded systems or performance-critical applications.
 *
 * @param x The x-coordinate difference between the two points.
 * @param y The y-coordinate difference between the two points.
 * @return s16 The angle in degrees between the two points.
 */
s16 calc_arctan2(s16 x, s16 y)
{
    static const unsigned char arctan_table[] = {
        0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 27,
        28, 29, 29, 30, 31, 31, 32, 33, 33, 34, 35, 35, 36, 36, 37, 37, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45};

    bool nx = (x < 0), ny = (y < 0);
    if (x < 0)
        x = -x;
    if (y < 0)
        y = -y;
    if (x < y)
        x = 90 - arctan_table[x * 64 / y];
    else
        x = arctan_table[y * 64 / x];
    if (nx)
    {
        if (180 - x > 127)
            x = 127;
        else
            x = 180 - x;
    }
    if (ny)
        x = -x;
    return x;
}
/**
 * @brief    Calculates the squared distance between two points.
 * @details  Computes the squared distance without the need for a square root operation, which is faster.
 *
 * @param    x1 The x-coordinate of the first point.
 * @param    y1 The y-coordinate of the first point.
 * @param    x2 The x-coordinate of the second point.
 * @param    y2 The y-coordinate of the second point.
 *
 * @return   The squared distance between the two points as an integer.
 */
int distance_squared(s16 x1, s16 y1, s16 x2, s16 y2)
{
    int dx = abs((s16)(x2 - x1));
    int dy = abs((s16)(y2 - y1));
    return dx * dx + dy * dy;
}

/**
 * @brief Calculates the integer square root of a number.
 * @details Implements an algorithm to find the integer square root of a given long integer 'x'
 *          without using the sqrt function. This is achieved by a binary search method.
 *
 * @param x The long integer number to find the square root of.
 * @return The integer square root of 'x'.
 */
int int_sqrt(long int x)
{
    int v_bit = 15;
    int n = 0;
    int b = 0x8000;

    if (x <= 1)
        return x;

    while (b)
    {
        int temp = ((n << 1) + b) << v_bit--;
        if (x >= temp)
        {
            n += b;
            x -= temp;
        }
        b >>= 1;
    }
    return n;
}

/**
 * @brief Clears the position data structures.
 * @details Resets the memory of the position data structures to zero, effectively clearing any stored data.
 */
void position_data_clear()
{
    memset(&func_data, 0, sizeof(func_data));
    memset(&seat_st, 0, sizeof(seat_st));
    Shape_data.interval = 0;
    Shape_data.type = WRONG_DATA;
}

/**
 * @brief Records position data if it meets certain conditions.
 * @param x1 The x-coordinate of the first point of the seat position.
 * @param y1 The y-coordinate of the first point of the seat position.
 * @param x2 The x-coordinate of the second point of the seat position.
 * @param y2 The y-coordinate of the second point of the seat position.
 * @param point_interval The interval at which points are recorded for the seat.
 * @return Returns 1 if the seat number is at maximum, otherwise returns 0 indicating successful recording.
 */
bool position_data_record(s16 x1, s16 y1, s16 x2, s16 y2, u32 point_interval)
{
    if (point_interval <= MIN_INTERVAL)
    {
        position_data_clear();
        Shape_data.interval = WRONG_INTERVAL;
        return false;
    }

    if (seat_st.seat_num == EFFE_DATA_LENGTH)
        return true;

    if ((x1 != seat_st.x1[seat_st.seat_num - 1]) ||
        (y1 != seat_st.y1[seat_st.seat_num - 1]) ||
        (x2 != seat_st.x2[seat_st.seat_num - 1]) ||
        (y2 != seat_st.y2[seat_st.seat_num - 1]) ||
        seat_st.seat_num == 0)
    {
        seat_st.x1[seat_st.seat_num] = x1;
        seat_st.y1[seat_st.seat_num] = y1;
        seat_st.x2[seat_st.seat_num] = x2;
        seat_st.y2[seat_st.seat_num] = y2;

        if (func_data.distance_flag == true && seat_st.seat_num == 0)
            seat_st.point_interval[seat_st.seat_num] = func_data.store_point;
        else
            seat_st.point_interval[seat_st.seat_num] = point_interval;

        seat_st.seat_num++;
    }

    return 0;
}

/**
 * @brief Recognizes the direction of movement based on old and new coordinates.
 * @details Determines the direction of a swipe or movement by comparing the previous (old) coordinates with the current (now) coordinates.
 *          The function checks if the movement is up, down, left, or right and sets the corresponding direction bit.
 *
 * @param old_x The x-coordinate of the previous position.
 * @param old_y The y-coordinate of the previous position.
 * @param now_x The x-coordinate of the current position.
 * @param now_y The y-coordinate of the current position.
 * @return A byte where each bit represents a direction: up, down, left, or right.
 */
u8 direction_recognit(s16 old_x, s16 old_y, s16 now_x, s16 now_y)
{
    u8 direction = 0;

    // up
    if (old_y < now_y)
        direction |= (1 << SWIPE_UP);
    // down
    if (old_y > now_y)
        direction |= (1 << SWIPE_DOWN);
    // left
    if (old_x > now_x)
        direction |= (1 << SWIPE_LEFT);
    // right
    if (old_x < now_x)
        direction |= (1 << SWIPE_RIGHT);

    return direction;
}

/**
 * @brief Determines the overall movement direction from recorded seat data.
 * @details This function analyzes the recorded seat movement data to determine the overall direction of movement.
 *          It checks for specific roll states and compares point intervals to detect zoom actions.
 *
 * @return Returns a value representing the overall movement direction:
 *         - SWIPE_UP if the up roll state is detected.
 *         - SWIPE_DOWN if the down roll state is detected.
 *         - SWIPE_LEFT if the left roll state is detected.
 *         - SWIPE_RIGHT if the right roll state is detected.
 *         - SWIPE_ZOOM_IN if a significant increase in point interval is detected, indicating a zoom in action.
 *         - SWIPE_ZOOM_OUT if a significant decrease in point interval is detected, indicating a zoom out action.
 *         - WRONG_DATA if none of the above conditions are met.
 */
u8 ret_overall_move_direction(void)
{
    if ((seat_st.arctan_coordinate_one >= 65) && seat_st.arctan_coordinate_two >= 65)
    {
        if (seat_st.up_num[2] == ROLL_STATE)
        {
            Shape_data.speed[0] = seat_st.y1[READ_STORE_LENGTH] - seat_st.y1[1];
            Shape_data.speed[1] = seat_st.y2[READ_STORE_LENGTH] - seat_st.y2[1];

            if ((Shape_data.speed[0] >= 55 - FASH_MOVE_SENSITIVE) || (Shape_data.speed[1] >= 55 - FASH_MOVE_SENSITIVE))
                Shape_data.fast_mode = true;
            else
                Shape_data.fast_mode = false;

            return SWIPE_UP;
        }
        else if (seat_st.down_num[2] == ROLL_STATE)
        {
            Shape_data.speed[0] = seat_st.y1[1] - seat_st.y1[READ_STORE_LENGTH];
            Shape_data.speed[1] = seat_st.y2[1] - seat_st.y2[READ_STORE_LENGTH];

            if ((Shape_data.speed[0] >= 55 - FASH_MOVE_SENSITIVE) || (Shape_data.speed[1] >= 55 - FASH_MOVE_SENSITIVE))
                Shape_data.fast_mode = true;
            else
                Shape_data.fast_mode = false;

            return SWIPE_DOWN;
        }
    }
    else if (seat_st.left_num[2] == ROLL_STATE)
    {
        Shape_data.speed[0] = seat_st.x1[1] - seat_st.x1[READ_STORE_LENGTH];
        Shape_data.speed[1] = seat_st.x2[1] - seat_st.x2[READ_STORE_LENGTH];

        if ((Shape_data.speed[0] >= 60 - FASH_MOVE_SENSITIVE) || (Shape_data.speed[1] >= 60 - FASH_MOVE_SENSITIVE))
            Shape_data.fast_mode = true;
        else
            Shape_data.fast_mode = false;

        return SWIPE_LEFT;
    }
    else if (seat_st.right_num[2] == ROLL_STATE)
    {
        Shape_data.speed[0] = seat_st.x1[READ_STORE_LENGTH] - seat_st.x1[1];
        Shape_data.speed[1] = seat_st.x2[READ_STORE_LENGTH] - seat_st.x2[1];

        if ((Shape_data.speed[0] >= 60 - FASH_MOVE_SENSITIVE) || (Shape_data.speed[1] >= 60 - FASH_MOVE_SENSITIVE))
            Shape_data.fast_mode = true;
        else
            Shape_data.fast_mode = false;

        return SWIPE_RIGHT;
    }
    else if (seat_st.point_interval[0] < seat_st.point_interval[seat_st.seat_num - 1])
    {
        if ((seat_st.point_interval[seat_st.seat_num - 1] - seat_st.point_interval[0]) >= 50)
        {
            printf("--->>> zoom large : %d\r\n", (seat_st.point_interval[seat_st.seat_num - 1] - seat_st.point_interval[0]));
            return SWIPE_ZOOM_IN;
        }
    }
    else if (seat_st.point_interval[0] > seat_st.point_interval[seat_st.seat_num - 1])
    {
        if ((seat_st.point_interval[0] - seat_st.point_interval[seat_st.seat_num - 1]) >= 40)
        {
            printf("--->>> zoom shirnk : %d\r\n", (seat_st.point_interval[0] - seat_st.point_interval[seat_st.seat_num - 1]));
            return SWIPE_ZOOM_OUT;
        }
    }

    return WRONG_DATA;
}

/**
 * @brief Calculates the offset direction based on seat position changes.
 * @details This function determines the predominant direction of movement (up, down, left, right)
 *          by comparing the initial and final positions of the seat. It increments counters for each direction
 *          based on the offset distance and the recognized direction.
 *
 * @return Returns an overall movement direction code as determined by the function ret_overall_move_direction().
 */
u8 position_offset()
{
    static s16 now_x, now_y;

#if 0 
    for (u8 i = 0; i < seat_st.seat_num - 1; i++)
    {
        // right and left
        if ((abs(seat_st.x1[i] - seat_st.x1[i + 1])) > abs((seat_st.y1[i] - seat_st.y1[i + 1])))
        {
            now_x = direction_recognit(seat_st.x1[i], seat_st.y1[i], seat_st.x1[i + 1], seat_st.y1[i + 1]);
            if ((now_x & 0x0C) == 0x08)
                seat_st.right_num[0]++;
            else
                seat_st.left_num[0]++;
        }
        // up and down
        else
        {
            now_y = direction_recognit(seat_st.x1[i], seat_st.y1[i], seat_st.x1[i + 1], seat_st.y1[i + 1]);
            if ((now_y & 0x03) == 0x01)
                seat_st.up_num[0]++;
            else
                seat_st.down_num[0]++;
        }

        // second point
        // right and left
        if ((abs(seat_st.x2[i] - seat_st.x2[seat_st.seat_num - 1])) > abs((seat_st.y2[i] - seat_st.y2[i + 1])))
        {
            now_x = direction_recognit(seat_st.x2[i], seat_st.y2[i], seat_st.x2[i + 1], seat_st.y2[i + 1]);
            if ((now_x & 0x0C) == 0x08)
                seat_st.right_num[1]++;
            else
                seat_st.left_num[1]++;
        }
        // up and down
        else
        {
            now_y = direction_recognit(seat_st.x2[i], seat_st.y2[i], seat_st.x2[i + 1], seat_st.y2[i + 1]);
            if ((now_y & 0x03) == 0x01)
                seat_st.up_num[1]++;
            else
                seat_st.down_num[1]++;
        }
    }
#endif

    // right and left
    if ((abs(seat_st.x1[0] - seat_st.x1[seat_st.seat_num - 1])) > abs((seat_st.y1[0] - seat_st.y1[seat_st.seat_num - 1])))
    {
        now_x = direction_recognit(seat_st.x1[0], seat_st.y1[0], seat_st.x1[seat_st.seat_num - 1], seat_st.y1[seat_st.seat_num - 1]);
        if ((now_x & 0x0C) == 0x08)
            seat_st.right_num[2]++;
        else
            seat_st.left_num[2]++;
    }
    // up and down
    else
    {
        now_y = direction_recognit(seat_st.x1[0], seat_st.y1[0], seat_st.x1[seat_st.seat_num - 1], seat_st.y1[seat_st.seat_num - 1]);
        if ((now_y & 0x03) == 0x01)
            seat_st.up_num[2]++;
        else
            seat_st.down_num[2]++;
    }

    // second point
    // right and left
    if ((abs(seat_st.x2[0] - seat_st.x2[seat_st.seat_num - 1])) > abs((seat_st.y2[0] - seat_st.y2[seat_st.seat_num - 1])))
    {
        now_x = direction_recognit(seat_st.x2[0], seat_st.y2[0], seat_st.x2[seat_st.seat_num - 1], seat_st.y2[seat_st.seat_num - 1]);
        if ((now_x & 0x0C) == 0x08)
            seat_st.right_num[2]++;
        else
            seat_st.left_num[2]++;
    }
    // up and down
    else
    {
        now_y = direction_recognit(seat_st.x2[0], seat_st.y2[0], seat_st.x2[seat_st.seat_num - 1], seat_st.y2[seat_st.seat_num - 1]);
        if ((now_y & 0x03) == 0x01)
            seat_st.up_num[2]++;
        else
            seat_st.down_num[2]++;
    }

    seat_st.arctan_coordinate_one = calc_arctan2(abs(seat_st.x1[0] - seat_st.x1[seat_st.seat_num - 1]), (abs(seat_st.y1[0] - seat_st.y1[seat_st.seat_num - 1])));
    seat_st.arctan_coordinate_two = calc_arctan2(abs(seat_st.x2[0] - seat_st.x2[seat_st.seat_num - 1]), (abs(seat_st.y2[0] - seat_st.y2[seat_st.seat_num - 1])));

    // printf("--->>> seat_st.arctan_coordinate_one :%d\r\n", seat_st.arctan_coordinate_one);
    // printf("--->>> seat_st.arctan_coordinate_two :%d\r\n", seat_st.arctan_coordinate_two);
    // printf("right_num[0]: %d, left_num[0]: %d, up_num[0]: %d, down_num[0]: %d\r\n", seat_st.right_num[0], seat_st.left_num[0], seat_st.up_num[0], seat_st.down_num[0]);
    // printf("right_num[1]: %d, left_num[1]: %d, up_num[1]: %d, down_num[1]: %d\r\n", seat_st.right_num[1], seat_st.left_num[1], seat_st.up_num[1], seat_st.down_num[1]);
    // printf("right_num[2]: %d, left_num[2]: %d, up_num[2]: %d, down_num[2]: %d\r\n", seat_st.right_num[2], seat_st.left_num[2], seat_st.up_num[2], seat_st.down_num[2]);

    return ret_overall_move_direction();
}

/**
 * @brief Maps the movement to a shape based on the coordinates provided.
 * @details This function calculates the squared distance between two points and uses it to determine the shape of the movement.
 *          It also checks for special conditions and records position data if certain criteria are met.
 *
 * @param x1 The x-coordinate of the first point.
 * @param y1 The y-coordinate of the first point.
 * @param x2 The x-coordinate of the second point.
 * @param y2 The y-coordinate of the second point.
 * @return Returns a code representing the shape of the movement or WRONG_DATA if the input is invalid or processing fails.
 */
Shape_ret_data move_shape_map(s16 x1, s16 y1, s16 x2, s16 y2)
{
    static int dist_squared, sqrt_squared;

    if (((x2 == 0xFFF) && (y2 == 0xFFF)) || ((x1 == 0x00) && (y1 == 0x00) && (x2 == 0x00) && (y2 == 0x00)))
    {
        position_data_clear();
        return Shape_data;
    }

    dist_squared = distance_squared(x1, y1, x2, y2);
    sqrt_squared = int_sqrt(dist_squared);
    Shape_data.interval = sqrt_squared;

    if (0x01 == position_data_record(x1, y1, x2, y2, sqrt_squared))
    {
        if (func_data.run_one == false)
        {
            func_data.run_one = true;
            Shape_data.type = position_offset();
            if (WRONG_DATA != Shape_data.type)
            {
                // printf("--->>> success \r\n");
                func_data.distance_flag = false;
                return Shape_data;
            }
            else
            {
                // printf("--->>> fail reset \r\n");
                if (func_data.distance_flag == false)
                {
                    func_data.distance_flag = true;
                    func_data.store_point = seat_st.point_interval[0];
                }
                func_data.run_one = false;
                memset(&seat_st, 0, sizeof(seat_st));
            }
        }
    }

    Shape_data.type = WRONG_DATA;

    return Shape_data;
}
#endif

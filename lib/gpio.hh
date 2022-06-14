#ifndef GPIO_HH__
#define GPIO_HH__

namespace openmsx {

// GPIO setup macros. 
// ------------------
// Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(_gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(_gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(_gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

// GPIO set,clr,get macros
// -----------------------
#define GPIO_SET *(_gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(_gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET *(_gpio+13) // gets   bits

class GPIO
{
    private:
        volatile unsigned *_gpio;

    public:
        bool            init ();
        void            set_register (uint16_t reg_index, uint32_t reg_value);
        uint32_t        get_register (uint16_t reg_index);

        inline void set_pins (uint32_t pins)
        {
            GPIO_SET = pins;
        }
        inline void clr_pins (uint32_t pins)
        {
            GPIO_CLR = pins;
        }
        inline uint32_t get_pins ()
        {
            return GPIO_GET;
        }
};

}

#endif //GPIO_HH__
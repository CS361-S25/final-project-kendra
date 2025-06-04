#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS

#include "emp/web/Animate.hpp"
#include "emp/web/web.hpp"
#include "World.h"


emp::web::Document doc("target");

class DEAnimator : public emp::web::Animate {

    // arena width and height
    const int num_h_boxes = 20;
    const int num_w_boxes = 20;
    const double RECT_SIDE = 28;
    const double width{num_w_boxes * RECT_SIDE};
    const double height{num_h_boxes * RECT_SIDE};

    emp::Random random{2};
    OrgWorld world{random};

    emp::web::Canvas canvas{width, height, "canvas"};

    public:

    DEAnimator() {
        // set up webpage
        doc << canvas;
        doc << GetToggleButton("Toggle");
        doc << GetStepButton("Step");
        
        // set up world
        random.ResetSeed(2);
        world.Resize(num_h_boxes,num_w_boxes);
        world.SetPopStruct_Grid(num_w_boxes, num_h_boxes);

        // insert 5 new organisms at random locations
        for (int i = 0; i< 5; i++) {
            Organism* new_org = new Organism(&world, world.MakeCellID());
            world.Inject(*new_org);
        }
        
    }

    /**
     * Draws the organism at the specified grid cell based on the tasks it's completed.
     * @param x Cell's x coordinate.
     * @param y Cell's y coordinate.
     * @param y Organism's index in the population.
     */
    void DrawOrg(int x, int y, int orgIndex) {
        emp::Ptr<Organism> currentOrg = world.GetOrgPtr(orgIndex);
        std::cout << currentOrg->GetMaxSent() << '\n';
        double normalizedMaxSent = currentOrg->GetMaxSent()/2147483647.0;
        std::string color = emp::ColorHSV(250*(1-normalizedMaxSent), 1, 1);
        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, color, color);
        //std::string mystr = std::to_string(currentOrg->GetCellID());
        //canvas.Text(emp::Point{(x * RECT_SIDE) +12, (y * RECT_SIDE) + 12}, mystr);
    }


    /**
     * Updates the organisms for each frame while animation is running or when step button is pressed.
     */
    void DoFrame() override {
        canvas.Clear();
        world.Update();
        int org_num = 0;
        for (int x = 0; x < num_w_boxes; x++){
            for (int y = 0; y < num_h_boxes; y++) {
                if (world.IsOccupied(org_num)) {
                    DrawOrg(x, y, org_num); // draws organism for an occupied cell
                    //emp::Ptr<Organism> currentOrg = world.GetOrgPtr(org_num);
                    //std::cout << world.minCellID << "\n";
                    //std::cout << "org at coords " << x << " " << y << " has ID " << currentOrg->GetCellID() << " and norm " << static_cast<float>(currentOrg->GetCellID() - world.minCellID)/static_cast<float>(world.maxCellID - world.minCellID) << '\n';
                } 
                else { // draws a white box for an unoccupied cell
                    canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "white", "black");
                }
                org_num++;
            }
        }
    }
};

DEAnimator animator;

int main() {animator.Step();}

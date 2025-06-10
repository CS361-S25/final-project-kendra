#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS

#include "emp/web/Animate.hpp"
#include "emp/web/web.hpp"
#include "World.h"

#include "ConfigSetup.h"
#include "emp/config/ArgManager.hpp"
#include "emp/prefab/ConfigPanel.hpp"
#include "emp/web/UrlParams.hpp"


emp::web::Document doc("target");
emp::web::Document settings("settings");
MyConfigType config;

class DEAnimator : public emp::web::Animate {

    // arena width and height
    const int num_h_boxes = 20;
    const int num_w_boxes = 20;
    const double RECT_SIDE = 28;
    const double width{num_w_boxes * RECT_SIDE};
    const double height{num_h_boxes * RECT_SIDE};

    emp::Random random{config.SEED()};
    OrgWorld world{random};

    emp::web::Canvas canvas{width, height, "canvas"};

    public:

    /**
     * Constructor for DEAnimator.
     */
    DEAnimator() {
        // sets up canvas, explanation text, and settings panel
        SetUpWebpage();
       
        // if user changed seed, updates it
        if (random.GetSeed() != config.SEED()) {
            random.ResetSeed(config.SEED());
        }

        // sets up world and population dimensions
        world.SetPopStruct_Grid(num_w_boxes, num_h_boxes);
        world.Resize(num_h_boxes,num_w_boxes);

         // creates the specified number of new organisms and randomly places them in the world
        for (int i = 0; i< config.NUM_START(); i++) {
            Organism* new_org = new Organism(&world, world.MakeCellID());
            world.Inject(*new_org);
        }
        
    }


     /**
     * Sets up canvas, explanation text, and settings panel for web GUI.
     */
    void SetUpWebpage() {
        // set up canvas + buttons
        doc << canvas;
        doc << GetToggleButton("Toggle");
        doc << GetStepButton("Step");

        // apply configuration query params and config files to config
        auto specs = emp::ArgManager::make_builtin_specs(&config);
        emp::ArgManager am(emp::web::GetUrlParams(), specs);
        am.UseCallbacks();
        if (am.HasUnused()) std::exit(EXIT_FAILURE);

        // setup configuration panel
        emp::prefab::ConfigPanel config_panel(config);
        config_panel.SetRange("NUM_START", "1", "9");

        // add explanation text and config panel
        settings << "<p>This project attempts to replicate the results of the paper 'Directed Evolution of Communication and Cooperation in Digital Organisms'. Each organism has a unique cell-ID and can send and retrieve messages. Although my project does not reward or punish organisms for the message value, the original paper attempts to evolve the behavior of organisms sending messages such that messages eventually carry the largest cell-ID. Each grid square represents an organism, and the coloration represents the largest message value that the organism has sent in the last 100 iterations. Red represents a larger value while blue represents a smaller value.</p>";
        settings << config_panel;
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

/* this file will formulate a cut flow table in the terminal for a given set of cuts

usage:
    std::vector<CutflowEntry> data = {
    {"Initial Events", util.GetNEvents()},
    {"is electron", n_electrons},
    {"is downstream", n_downstream},
    {"trkqual.result > 0.2", n_trkqual},
    {"t0err > 0.9 ns", n_t0err},
    {"|d0| < 100 mm ", n_d0},
    {"450 < rmax < 680 mm ", n_rmax},
    {"has CRV coincidence", n_crv}
    };

    printCutflowTable(data);
    
*/

#ifndef cutflow_hh_
#define cutflow_hh_
#include <format>
// Structure to hold cutflow data
struct CutflowEntry {
    std::string cut_name;
    long long passed_count;
};

void printCutflowTable(const std::vector<CutflowEntry>& entries) {
    if (entries.empty()) {
        std::cout << "No cutflow data to display." << std::endl;
        return;
    }
    
    // Define column widths
    const int cut_name_width = 30;
    const int passed_width = 15;
    const int prev_change_width = 15;
    const int initial_change_width = 15;

    // Print the table header
    std::cout << std::format("{:<{}} {:>{}} {:>{}} {:>{}}\n",
                             "Cut", cut_name_width,
                             "Passed", passed_width,
                             "% Change (Prev)", prev_change_width,
                             "% Change (Initial)", initial_change_width);

    // Print a separator line. std::format does not have the fill-character side effect.
    std::cout << std::format("{:-<{}}\n", "", cut_name_width + passed_width + prev_change_width + initial_change_width + 3);

    const long long initial_count = entries[0].passed_count;
    long long last_count = 0;

    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        
        std::string percent_prev_str;
        if (i > 0 && last_count > 0) {
            double percent_change_prev = 100 - fabs(static_cast<double>(entry.passed_count - last_count) ) / last_count * 100.0;
            percent_prev_str = std::format("{:+.2f}%", percent_change_prev);
        } else {
            percent_prev_str = "N/A";
        }
        
        std::string percent_initial_str;
        if (initial_count > 0) {
            double percent_change_initial = 100 - fabs(static_cast<double>(entry.passed_count - initial_count)) / initial_count * 100.0;
            percent_initial_str = std::format("{:+.2f}%", percent_change_initial);
        } else {
            percent_initial_str = "N/A";
        }

        std::cout << std::format("{:<{}} {:>{}} {:>{}} {:>{}}\n", 
                                 entry.cut_name, cut_name_width,
                                 entry.passed_count, passed_width,
                                 percent_prev_str, prev_change_width,
                                 percent_initial_str, initial_change_width);
        
        last_count = entry.passed_count;
    }
}

#endif


#include "database.h"
#include <fmt/core.h>
void database_methods::print_table_details(Table& table) {
    std::vector<std::string> column_names;

    for (auto i : table.columns) {
        column_names.push_back(i.type.name);
    }

    auto max_size = column_names.size() * 7;
    for (auto i : column_names) {
        max_size += i.size();
    }

    for (auto i = 0; i < max_size; i++) {
        fmt::print("=");
    }
    fmt::print("\nTable Name: {}\n", table.name);
    for (auto i = 0; i < max_size; i++) {
        fmt::print("-");
    }
    fmt::print("\n");
    for (auto i : column_names) {
        fmt::print("   {}   |", i);
    }
    fmt::print("\n");
    for (auto i = 0; i < max_size; i++) {
        fmt::print("=");
    }
    fmt::print("\n");

    for (auto j = 0; j < table.columns[0].values.size(); j++) {
        for (auto i = 0; i < table.columns.size(); i++) {
            fmt::print("   {}    ", table.columns[i].values[j]);
        }
        fmt::print("\n");

    }

}

//Ukradzione z chatGPT
void database_methods::print_mixed_tables(std::vector<std::pair<std::string, std::string>> table_column, std::vector<Table> &tables_data) {
    size_t ncols = table_column.size();
    if (ncols == 0) {
        fmt::print("[ No columns selected ]\n");
        return;
    }

    // 1) Resolve each requested column into a pointer to its Column struct
    struct ColInfo {
        const Column* col;     // pointer into tables_data
        std::string header;    // "(TYPE) colname.tablename"
    };
    std::vector<ColInfo> cols;

    for (auto &tc : table_column) {
        const auto &tbl_name = tc.first;
        const auto &col_name = tc.second;
        // find the Table
        auto tbl_it = std::find_if(
            tables_data.begin(), tables_data.end(),
            [&](auto &T){ return T.name == tbl_name; }
        );
        if (tbl_it == tables_data.end()) {
            throw std::runtime_error("Table not found: " + tbl_name);
        }
        // find the Column
        auto &cols_vec = tbl_it->columns;
        auto col_it = std::find_if(
            cols_vec.begin(), cols_vec.end(),
            [&](auto &C){ return C.type.name == col_name; }
        );
        if (col_it == cols_vec.end()) {
            throw std::runtime_error(
                "Column not found: " + tbl_name + "." + col_name
            );
        }
        // build header string
        std::string hdr = fmt::format(
            "({}) {}.{}",
            col_it->type.type,
            col_it->type.name,
            tbl_name
        );
        cols.push_back({ &*col_it, std::move(hdr) });
    }

    // 2) Determine row count (max of all columns)
    size_t nrows = 0;
    for (auto &ci : cols) {
        nrows = std::max(nrows, ci.col->values.size());
    }

    // 3) Compute display width for each column
    std::vector<size_t> width(ncols);
    for (size_t i = 0; i < ncols; ++i) {
        // start with header width
        width[i] = cols[i].header.size();
        // expand for each cell in that column
        for (auto &cell : cols[i].col->values) {
            width[i] = std::max(width[i], cell.size());
        }
        // optional cap to avoid runaway widths
        width[i] = std::min(width[i], size_t(60));
    }

    // 4) Build a separator line
    auto make_sep = [&]() {
        std::string s = "+";
        for (size_t i = 0; i < ncols; ++i) {
            s += std::string(width[i] + 2, '-') + "+";
        }
        return s;
    };
    std::string sep = make_sep();

    // 5) Print header
    fmt::print("{}\n", sep);
    fmt::print("|");
    for (size_t i = 0; i < ncols; ++i) {
        // left-align header in its column
        fmt::print(" {:{}} |", cols[i].header, width[i]);
    }
    fmt::print("\n{}\n", sep);

    // 6) Print data rows
    for (size_t r = 0; r < nrows; ++r) {
        fmt::print("|");
        for (size_t i = 0; i < ncols; ++i) {
            const auto &values = cols[i].col->values;
            std::string cell = (r < values.size() ? values[r] : "");
            fmt::print(" {:{}} |", cell, width[i]);
        }
        fmt::print("\n");
    }

    // 7) Bottom border
    fmt::print("{}\n", sep);
}

//Wzorowane tamtym ukradzionym ale moje juz
void database_methods::print_table(const Table table) {
    size_t ncols = table.columns.size();
    if (ncols == 0) {
        fmt::print("[ Table is empty ]\n");
        return;
    }

    // 1) Compute max width per column
    std::vector<size_t> width(ncols);
    for (size_t c = 0; c < ncols; ++c) {
        auto& col = table.columns[c];
        std::string header = fmt::format("{} - ({})", col.type.name, col.type.type);
        width[c] = header.size();
        for (auto& cell : col.values) {
            width[c] = std::max(width[c], cell.size());
        }
        // Optional cap to avoid runaway widths:
        width[c] = std::min(width[c], size_t(50));
    }

    // 2) Build a separator string
    auto make_sep = [&]() {
        std::string sep = "+";
        for (size_t c = 0; c < ncols; ++c) {
            sep += std::string(width[c] + 2, '-') + "+";
        }
        return sep;
    };

    // 3) Print table name and top border
    fmt::print("\n[ Table Name: {} ]\n", table.name);
    std::string sep = make_sep();
    fmt::print("{}\n", sep);

    // 4) Header row
    fmt::print("|");
    for (size_t c = 0; c < ncols; ++c) {
        auto& col = table.columns[c];
        std::string hdr = fmt::format("{} - ({})", col.type.name, col.type.type);
        // left‑align header
        fmt::print(" {:{}} |", hdr, width[c]);
    }
    fmt::print("\n{}", sep, "\n");

    fmt::print("\n");
    // 5) Data rows
    size_t nrows = 0;
    for (auto& col : table.columns)
        nrows = std::max(nrows, col.values.size());

    for (size_t r = 0; r < nrows; ++r) {
        fmt::print("|");
        for (size_t c = 0; c < ncols; ++c) {
            auto& col = table.columns[c];
            std::string cell = (r < col.values.size() ? col.values[r] : "");
            fmt::print(" {:{}} |", cell, width[c]);
        }
        fmt::print("\n");
    }

    // 6) Bottom border
    fmt::print("{}\n", sep);
}

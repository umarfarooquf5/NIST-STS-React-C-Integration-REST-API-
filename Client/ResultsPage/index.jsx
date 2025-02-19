import React, { useEffect, useState } from "react";
import {
  Box,
  Typography,
  Paper,
  CircularProgress,
  Button,
} from "@mui/material";
import { DataGrid } from "@mui/x-data-grid";
import { toast, ToastContainer } from "react-toastify";
import "react-toastify/dist/ReactToastify.css";

const ResultsPage = () => {
  const [results, setResults] = useState(null);
  const [error, setError] = useState(null);

  useEffect(() => {
    fetch("http://192.168.18.30:8080/result")
      .then((response) => {
        if (!response.ok) {
          throw new Error("Failed to fetch result data");
        }
        return response.json();
      })
      .then((data) => {
        setResults(data);
        toast.success("Results fetched successfully!");
      })
      .catch((error) => {
        setError(error.message);
        toast.error("Error fetching results!");
      });
  }, []);

  if (error) {
    return (
      <Box
        sx={{
          maxWidth: "800px",
          margin: "auto",
          padding: "20px",
          textAlign: "center",
        }}
      >
        <Typography variant="h6" color="error">
          Error: {error}
        </Typography>
      </Box>
    );
  }

  if (!results) {
    return (
      <Box
        sx={{
          display: "flex",
          justifyContent: "center",
          alignItems: "center",
          height: "80vh",
        }}
      >
        <CircularProgress color="success" />
        <Typography variant="h6" color="primary" sx={{ marginLeft: "16px" }}>
          Loading...
        </Typography>
      </Box>
    );
  }

  const columns = [
    {
      field: "segment_no",
      headerName: "Segment No.",
      flex: 1,
      headerAlign: "center",
      align: "center",
    },
    {
      field: "test_name",
      headerName: "Test Name",
      flex: 2,
      headerAlign: "center",
      align: "center",
    },
    {
      field: "p_value",
      headerName: "P-Value",
      flex: 1,
      headerAlign: "center",
      align: "center",
    },
    {
      field: "timestamp",
      headerName: "Timestamp",
      flex: 2,
      headerAlign: "center",
      align: "center",
    },
    {
      field: "result",
      headerName: "Result",
      flex: 1,
      headerAlign: "center",
      align: "center",
      renderCell: (params) => (
        <span
          style={{
            color: params.row.p_value >= 0.01 ? "#198754" : "#d32f2f",
            fontWeight: "bold",
          }}
        >
          {params.row.p_value >= 0.01 ? "Pass" : "Fail"}
        </span>
      ),
    },
  ];

  const rows = results.results.map((result, index) => ({
    id: index,
    segment_no: result.segment_no,
    test_name: result.test_name || "N/A",
    p_value: result.p_value,
    timestamp: result.timestamp.trim(),
  }));

  return (
    <Box
      p={4}
      sx={{
        maxWidth: "1250px",
        margin: "auto",
        border: "1px solid #ccc",
        borderRadius: "8px",
        backgroundColor: "#f9f9f9",
        boxShadow: "0 4px 8px rgba(0, 0, 0, 0.1)",
      }}
    >
      <Box
        display="flex"
        justifyContent="space-between"
        alignItems="center"
        mb={3}
      >
        <Typography
          variant="h4"
          sx={{
            color: "#198754",
            fontWeight: "bold",
          }}
        >
          Test Results
        </Typography>
        <Button
          variant="contained"
          sx={{
            backgroundColor: "#198754",
            color: "#fff",
            "&:hover": { backgroundColor: "#146c43" },
          }}
        >
          Submit Results
        </Button>
      </Box>

      <Box
        mb={3}
        p={2}
        sx={{
          backgroundColor: "#e9f7ef",
          borderRadius: "8px",
          boxShadow: "0 4px 8px rgba(0, 0, 0, 0.1)",
        }}
      >
        <Typography variant="h6" sx={{ color: "#198754", fontWeight: "bold" }}>
          File Information
        </Typography>
        <Typography variant="body1">
          <strong>Filename:</strong> {results.filename || "N/A"}
        </Typography>
        <Typography variant="body1">
          <strong>Number of Segments:</strong>{" "}
          {results.number_of_segments || "N/A"}
        </Typography>
        <Typography variant="body1">
          <strong>Segment Size:</strong> {results.segment_size || "N/A"}
        </Typography>
      </Box>

      <Paper
        sx={{
          marginTop: "20px",
          padding: "10px",
          backgroundColor: "#fff",
          borderRadius: "8px",
          boxShadow: "0 4px 8px rgba(0, 0, 0, 0.1)",
        }}
      >
        <div style={{ height: "70vh", width: "100%" }}>
          <DataGrid
            rows={rows}
            columns={columns}
            pageSize={10}
            rowsPerPageOptions={[10]}
            sx={{
              "& .MuiDataGrid-columnHeaders": {
                backgroundColor: "#198754",
                color: "#fff",
                fontWeight: "bold",
              },
              "& .MuiDataGrid-cell": {
                textAlign: "center",
              },
              "& .MuiDataGrid-cell:hover": {
                backgroundColor: "#e9f5ea",
              },
              "& .MuiDataGrid-footerContainer": {
                backgroundColor: "#f9f9f9",
              },
            }}
          />
        </div>
      </Paper>

      <ToastContainer position="top-right" autoClose={3000} />
    </Box>
  );
};

export default ResultsPage;

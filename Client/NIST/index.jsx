import React, { useState } from "react";
import {
  Box,
  Button,
  TextField,
  Typography,
  FormControl,
  FormGroup,
  FormControlLabel,
  Checkbox,
  Grid,
  InputBase,
  CircularProgress,
} from "@mui/material";
import { toast, ToastContainer } from "react-toastify";
import "react-toastify/dist/ReactToastify.css";
import { useNavigate } from "react-router-dom";

const NIST = () => {
  const [file, setFile] = useState(null);
  const [numberOfSegments, setNumberOfSegments] = useState("");
  const [segmentSize, setSegmentSize] = useState("1048576");
  const [selectedTasks, setSelectedTasks] = useState([]);
  const [isTestRunning, setIsTestRunning] = useState(false);
  const [selectAll, setSelectAll] = useState(false);
  const navigate = useNavigate();

  const tasks = [
    "Block Frequency (Block Length: 128 bits)",
    "Overlapping Templates (Template Length: 9 bits)",
    "Non-overlapping Templates (Template Length: 9 bits)",
    "Serial (Block Length: 16 bits)",
    "Approximate Entropy (Block Length: 10 bits)",
    "Linear Complexity (Block Length: 500 bits)",
    "Universal (Block Length: 1280 bits)",
    "Cumulative Sums",
    "Runs",
    "Longest Runs of Ones",
    "Rank",
    "Spectral DFT",
    "Random Excursions",
    "Random Excursions Variant",
    "Frequency",
  ];

  const handleCheckboxChange = (task) => {
    setSelectedTasks((prevTasks) =>
      prevTasks.includes(task)
        ? prevTasks.filter((t) => t !== task)
        : [...prevTasks, task]
    );
  };

  const handleSelectAllChange = () => {
    if (selectAll) {
      setSelectedTasks([]);
    } else {
      setSelectedTasks(tasks);
    }
    setSelectAll(!selectAll);
  };

  const handleFileChange = (e) => {
    const selectedFile = e.target.files[0];
    if (selectedFile) {
      const fileSizeBits = selectedFile.size * 8;
      if (fileSizeBits < 1000000) {
        toast.error("File size must be at least 1 million bits.");
        setFile(null);
        return;
      }
      setFile(selectedFile);
      const segments = Math.floor(fileSizeBits / parseInt(segmentSize));
      setNumberOfSegments(segments);
    } else {
      setFile(null);
    }
  };

  const handleRunTest = async () => {
    if (
      !file ||
      !numberOfSegments ||
      !segmentSize ||
      selectedTasks.length === 0
    ) {
      toast.error("Please complete all fields and select at least one task.");
      return;
    }

    const fileSizeBits = file.size * 8;
    const maxSegments = Math.floor(fileSizeBits / parseInt(segmentSize));
    if (parseInt(numberOfSegments) > maxSegments) {
      toast.error(
        `Number of segments cannot exceed ${maxSegments}, or you can adjust Segment Size if you want specific number of segments.`
      );
      return;
    }

    const additionalData = {
      numberOfSegments,
      segmentSize,
      selectedTasks,
    };

    const additionalFile = new Blob([JSON.stringify(additionalData)], {
      type: "text/plain",
    });

    const formData = new FormData();
    formData.append("binary_file", file);
    formData.append("additional_file", additionalFile);

    try {
      setIsTestRunning(true);

      const response = await fetch("http://192.168.18.30:8080/NIST", {
        method: "POST",
        body: formData,
      });

      const result = await response.json();

      setIsTestRunning(false);

      if (response.ok && result.status === "success") {
        toast.success("Results received successfully!");
        navigate("/results", { state: { resultFile: result.result_file } });
      } else {
        toast.error("Something went wrong while processing the test.");
      }
    } catch (error) {
      toast.error("Error while running the test: " + error.message);
      setIsTestRunning(false);
    }
  };

  return (
    <Box sx={{ display: "flex", flexDirection: "column", gap: "10px" }} p={4}>
      <Typography variant="h2" gutterBottom sx={{ color: "#198754" }}>
        NIST Statistical Test Suite
      </Typography>

      <Grid container spacing={3}>
        <Grid item xs={12} md={4}>
          <FormControl
            margin="normal"
            sx={{
              width: "100%",
              border: "1px solid #bab8b8",
              padding: "5px",
              borderRadius: "4px",
            }}
          >
            <InputBase
              type="file"
              onChange={handleFileChange}
              sx={{
                padding: "6px",
                border: "none",
                outline: "none",
                cursor: "pointer",
              }}
            />
          </FormControl>
        </Grid>

        <Grid item xs={12} md={4}>
          <TextField
            sx={{ width: "100%" }}
            margin="normal"
            label="Number of Segments"
            value={numberOfSegments}
            onChange={(e) => setNumberOfSegments(e.target.value)}
            type="number"
          />
        </Grid>

        <Grid item xs={12} md={4}>
          <TextField
            sx={{ width: "100%" }}
            margin="normal"
            label="Segment Size (in Bits)"
            value={segmentSize}
            onChange={(e) => setSegmentSize(e.target.value)}
            type="number"
          />
        </Grid>

        <Grid item xs={12}>
          <FormGroup>
            <Grid container spacing={1}>
              <Grid item xs={12}>
                <FormControlLabel
                  control={
                    <Checkbox
                      sx={{
                        "&.Mui-checked": {
                          color: "#198754",
                        },
                        padding: "2px",
                      }}
                      checked={selectAll}
                      onChange={handleSelectAllChange}
                    />
                  }
                  label="Select All"
                />
              </Grid>
              {tasks.map((task) => (
                <Grid item xs={6} sm={4} key={task}>
                  <FormControlLabel
                    control={
                      <Checkbox
                        sx={{
                          "&.Mui-checked": {
                            color: "#198754",
                          },
                          padding: "2px",
                        }}
                        checked={selectedTasks.includes(task)}
                        onChange={() => handleCheckboxChange(task)}
                      />
                    }
                    label={task}
                  />
                </Grid>
              ))}
            </Grid>
          </FormGroup>
        </Grid>

        <Grid item xs={12}>
          <Button
            variant="contained"
            sx={{
              width: "100px",
              mt: 2,
              backgroundColor: "#198754",
              "&:hover": { backgroundColor: "#198754" },
            }}
            onClick={handleRunTest}
            disabled={isTestRunning}
          >
            {isTestRunning ? <CircularProgress size={24} /> : "Run Test"}
          </Button>
        </Grid>
      </Grid>
      <ToastContainer position="top-right" autoClose={5000} />
    </Box>
  );
};

export default NIST;
